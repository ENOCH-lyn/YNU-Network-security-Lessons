"""多线程并发聊天室服务器端

主线程负责 accept 新连接，每来一个客户端就开一个线程单独处理（一连接一线程）
所有线程共享同一个 ChatRoom，靠锁保护在线用户表，实现广播、私聊、在线列表等功能
"""

import argparse
import socket
import threading
from dataclasses import dataclass, field

from chat_protocol import JsonLineParser, ProtocolError, encode_packet


@dataclass
class ClientConnection:
    """一个客户端连接的封装：socket、地址、用户名和发送锁。"""

    sock: socket.socket
    address: tuple
    username: str | None = None
    # 多个线程（如广播时）可能同时往同一个 socket 写，加锁保证一个包不被另一个包穿插
    send_lock: threading.Lock = field(default_factory=threading.Lock)

    def send(self, packet):
        data = encode_packet(packet)
        with self.send_lock:
            self.sock.sendall(data)


class ChatRoom:
    """聊天室：维护在线用户表，提供登录、登出、广播、私聊等操作"""

    def __init__(self):
        # 用户名 -> ClientConnection 的映射，即在线用户表
        self.users = {}
        # 使用锁保护 users，避免并发访问导致数据不一致或崩溃
        # RLock 允许同一线程在持锁的情况下再次获取锁，避免 send 触发的 logout 导致死锁
        self.lock = threading.RLock()

    def login(self, client, username):
        """登录：校验用户名、占用名额，广播上线通知"""
        # 规整用户名，校验合法性
        username = self._clean_username(username)
        # 加锁
        with self.lock:
            # 检查用户名是否已被占用，若是则拒绝登录
            if username in self.users:
                client.send({"type": "error", "text": f"username already exists: {username}"})
                return False
            # 把用户加入在线表，并把用户名记录在连接对象里
            self.users[username] = client
            client.username = username

        # 发送登录成功的系统消息
        client.send({"type": "system", "text": f"login success, welcome {username}"})
        # 通知其他人有新用户上线
        self.broadcast_event({"type": "user_join", "username": username}, exclude=username)
        return True

    def logout(self, client):
        """登出：从在线表移除并广播下线通知"""
        username = client.username
        if not username:
            return
        removed = False
        # 加锁
        with self.lock:
            # 判断 self.users.get(username) is client，避免误删同名的新连接
            if self.users.get(username) is client:
                del self.users[username]
                client.username = None
                removed = True

        if removed:
            # 如果移除，则广播用户离线事件
            self.broadcast_event({"type": "user_leave", "username": username})

    def broadcast_message(self, sender, text):
        """群聊：把 sender 的消息广播给所有在线用户"""
        # 检验消息格式
        text = str(text).strip()
        if not text:
            sender.send({"type": "error", "text": "message text cannot be empty"})
            return
        # 广播消息
        self.broadcast_event({"type": "message", "from": sender.username, "text": text})

    def send_private(self, sender, target_name, text):
        """私聊"""
        # 检验消息格式
        text = str(text).strip()
        target_name = str(target_name).strip()
        if not target_name or not text:
            sender.send({"type": "error", "text": "private message requires target and text"})
            return

        with self.lock:
            # 查找目标用户连接对象
            target = self.users.get(target_name)

        # 如果目标用户不在线，发送错误消息给 sender    
        if target is None:
            sender.send({"type": "error", "text": f"user not online: {target_name}"})
            return
        
        # 发送私聊消息给目标用户，并发送确认消息给 sender
        target.send({"type": "private", "from": sender.username, "text": text})
        sender.send({"type": "private_ack", "to": target_name, "text": text})

    def send_online_list(self, client):
        """返回当前在线用户名列表。"""
        with self.lock:
            # 返回排序后的用户名列表
            users = sorted(self.users.keys())
        # 发送在线用户列表给请求的客户端
        client.send({"type": "online", "users": users})

    def broadcast_event(self, packet, exclude=None):
        """把一个包发给所有在线用户，可用 exclude 排除某个用户名"""
        # 复制一份在线用户连接列表，避免在发送过程中 users 被修改导致 KeyError 或漏发
        with self.lock:
            clients = [
                user_client
                for username, user_client in self.users.items()
                if username != exclude
            ]
        # 把包发给所有在线用户
        for client in clients:
            try:
                client.send(packet)
            except OSError:
                # 某个连接已断开，顺手把它登出清理掉。
                self.logout(client)

    def _clean_username(self, username):
        """校验并规整用户名：非空、不超长、不含空白字符"""
        username = str(username).strip()
        if not username:
            raise ValueError("username cannot be empty")
        if len(username) > 20:
            raise ValueError("username is too long")
        if any(ch.isspace() for ch in username):
            raise ValueError("username cannot contain whitespace")
        return username


class ChatServer:
    """服务器核心：监听端口、accept 连接、为每个客户端分派处理线程"""

    def __init__(self, host="0.0.0.0", port=9009, backlog=50):
        # 设置服务器监听地址和端口
        self.host = host
        self.port = port
        # backlog 作为 listen 的参数，指定未完成连接排队的最大数量
        self.backlog = backlog
        # 创建一个 ChatRoom 实例
        self.room = ChatRoom()
        self.sock = None
        # running 控制主循环是否继续；ready 供测试等待服务器就绪。
        self.running = threading.Event()
        self.ready = threading.Event()
        self.client_threads = []

    def serve_forever(self):
        """创建监听 socket 并进入 accept 主循环，直到 running 被清除"""
        # 创建 TCP socket
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # SO_REUSEADDR 允许服务器重启后立刻复用端口，避免 TIME_WAIT 占用导致 bind 失败
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        # 绑定地址和端口，开始监听
        self.sock.bind((self.host, self.port))
        self.sock.listen(self.backlog)
        # 设超时让 accept 不会永久阻塞，从而能周期性检查 running 实现退出
        self.sock.settimeout(0.2)
        # 若传入 port=0，系统会分配随机端口，因此需要获取实际绑定的端口号并更新 self.port
        self.port = self.sock.getsockname()[1]
        # 设置 running 和 ready，表示服务器已启动并进入主循环
        self.running.set()
        self.ready.set()
        print(f"Chat server listening on {self.host}:{self.port}")

        try:
            # 主循环：不断 accept 新连接，直到 running 被清除或 socket 被关闭
            while self.running.is_set():
                try:
                    # accept 返回一个新的 socket 和客户端地址
                    client_sock, address = self.sock.accept()
                except socket.timeout:
                    # accept 超时，继续循环检查 running 是否被清除
                    continue
                except OSError:
                    # socket 被关闭，退出主循环
                    break

                # 每个连接交给一个守护线程处理，主线程继续 accept 下一个连接
                client = ClientConnection(client_sock, address)
                thread = threading.Thread(
                    target=self.handle_client,
                    args=(client,),
                    daemon=True,
                )
                self.client_threads.append(thread)
                thread.start()
        finally:
            # 清理：关闭监听 socket，清除 running 和 ready，等待所有客户端线程结束
            self.running.clear()
            self.ready.clear()
            if self.sock is not None:
                try:
                    self.sock.close()
                except OSError:
                    pass

    def stop(self):
        """停止服务器：清除 running 并关闭监听 socket"""
        self.running.clear()
        if self.sock is not None:
            try:
                self.sock.close()
            except OSError:
                pass

    def handle_client(self, client):
        """单个客户端的处理线程：循环收数据、解析、分派，直到断开或退出"""
        parser = JsonLineParser()
        # 设超时让 recv 不会永久阻塞，从而能周期性检查 running
        client.sock.settimeout(0.5)

        try:
            # 首先发送提示消息，要求登录
            client.send({"type": "system", "text": "connected, please login"})
            while self.running.is_set():
                try:
                    # 从客户端 socket 接收数据
                    chunk = client.sock.recv(4096)
                except socket.timeout:
                    continue

                # recv 返回空字节串表示连接已关闭，退出循环
                if not chunk:
                    break

                # 把字节交给解析器
                packets = parser.feed(chunk)
                # 解析器可能返回一个或多个完整包，逐个分派处理；如果处理结果是 "quit"，则退出循环
                for packet in packets:
                    if self._dispatch_packet(client, packet) == "quit":
                        return
        except ProtocolError as exc:
            self._safe_send(client, {"type": "error", "text": f"protocol error: {exc}"})
        except (ConnectionResetError, OSError):
            pass
        finally:
            # 确保登出并关闭连接，清理资源
            self.room.logout(client)
            try:
                client.sock.close()
            except OSError:
                pass

    def _dispatch_packet(self, client, packet):
        """根据包的 type 分派到对应处理逻辑；返回 "quit" 表示该退出"""
        packet_type = packet["type"]

        if packet_type == "login":
            if client.username:
                client.send({"type": "error", "text": "already logged in"})
                return None
            try:
                self.room.login(client, packet.get("username", ""))
            except ValueError as exc:
                client.send({"type": "error", "text": str(exc)})
            return None

        if packet_type == "quit":
            client.send({"type": "system", "text": "bye"})
            return "quit"

        # 除登录和退出外，其余操作都要求先登录
        if not client.username:
            client.send({"type": "error", "text": "please login first"})
            return None

        if packet_type == "broadcast":
            self.room.broadcast_message(client, packet.get("text", ""))
        elif packet_type == "private":
            self.room.send_private(client, packet.get("to", ""), packet.get("text", ""))
        elif packet_type == "list":
            self.room.send_online_list(client)
        else:
            client.send({"type": "error", "text": f"unknown command: {packet_type}"})
        return None

    def _safe_send(self, client, packet):
        """安全发送，忽略发送失败"""
        try:
            client.send(packet)
        except OSError:
            pass


def main():
    parser = argparse.ArgumentParser(description="Multi-threaded TCP chat server")
    parser.add_argument("--host", default="0.0.0.0")
    parser.add_argument("--port", type=int, default=9009)
    args = parser.parse_args()

    server = ChatServer(host=args.host, port=args.port)
    try:
        # 启动服务器，直到 Ctrl+C 触发 KeyboardInterrupt
        server.serve_forever()
    except KeyboardInterrupt:
        # Ctrl+C 时停止
        print("\nserver stopped")
        server.stop()


if __name__ == "__main__":
    main()
