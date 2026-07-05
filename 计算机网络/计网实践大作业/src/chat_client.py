"""命令行聊天室客户端

两个线程：主线程读键盘输入、把命令编码后发给服务器
接收线程不断 recv 服务器推送的消息并打印
"""

import argparse
import socket
import sys
import threading

from chat_protocol import JsonLineParser, ProtocolError, encode_packet


def format_packet(packet):
    """把服务器发来的 JSON 包格式化成一行便于阅读的文本"""
    packet_type = packet.get("type")
    if packet_type == "system":
        return f"[SYSTEM] {packet.get('text', '')}"
    if packet_type == "error":
        return f"[ERROR] {packet.get('text', '')}"
    if packet_type == "message":
        return f"[{packet.get('from')}] {packet.get('text', '')}"
    if packet_type == "private":
        return f"[PRIVATE from {packet.get('from')}] {packet.get('text', '')}"
    if packet_type == "private_ack":
        return f"[PRIVATE to {packet.get('to')}] {packet.get('text', '')}"
    if packet_type == "online":
        return "[ONLINE] " + ", ".join(packet.get("users", []))
    if packet_type == "user_join":
        return f"[JOIN] {packet.get('username')} is online"
    if packet_type == "user_leave":
        return f"[LEAVE] {packet.get('username')} is offline"
    # 未知类型原样输出
    return f"[RAW] {packet}"


def receive_loop(sock, stop_event):
    """接收线程：循环读取服务器数据，解析后打印"""
    parser = JsonLineParser()
    while not stop_event.is_set():
        # 接收服务器发来的数据
        try:
            chunk = sock.recv(4096)
        except OSError:
            break

        if not chunk:
            # 服务器关闭了连接
            break

        try:
            # 解析出其中的一个或多个完整包
            packets = parser.feed(chunk)
        except ProtocolError as exc:
            print(f"[ERROR] protocol error: {exc}")
            break

        # 打印所有解析成功的包
        for packet in packets:
            print(format_packet(packet))

    # 收到结束信号，退出循环
    stop_event.set()


def build_packet_from_input(line):
    """把用户输入的一行文本翻译成要发送的协议包

    以 '/' 开头的是命令（/login、/list、/quit、/w），
    其余文本一律当作群聊消息
    """
    # 去掉首尾空白，忽略空行
    line = line.strip()
    if not line:
        return None
    # 命令解析：/login name, /list, /quit, /w name message
    if line.startswith("/login "):
        return {"type": "login", "username": line.split(maxsplit=1)[1]}
    if line == "/list":
        return {"type": "list"}
    if line == "/quit":
        return {"type": "quit"}
    if line.startswith("/w "):
        # /w 目标用户 消息内容，最多切成 3 段
        parts = line.split(maxsplit=2)
        if len(parts) < 3:
            print("usage: /w username message")
            return None
        return {"type": "private", "to": parts[1], "text": parts[2]}
    # 以 '/' 开头但不符合已知命令格式，提示未知命令
    if line.startswith("/"):
        print("unknown command")
        return None
    # 未命中任何已知命令，当作普通消息处理
    return {"type": "broadcast", "text": line}


def main():
    parser = argparse.ArgumentParser(description="Command line chat client")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=9009)
    args = parser.parse_args()

    # 连接服务器
    sock = socket.create_connection((args.host, args.port))
    # 用于通知接收线程退出
    stop_event = threading.Event()
    # 接收消息放到独立守护线程
    thread = threading.Thread(target=receive_loop, args=(sock, stop_event), daemon=True)
    thread.start()

    print("Commands: /login name, /w name message, /list, /quit")
    try:
        # 未结束则循环读用户输入，翻译成协议包发给服务器
        while not stop_event.is_set():
            line = sys.stdin.readline()
            if not line:
                # 输入流结束（如 Ctrl+Z / Ctrl+D）
                break
            # 把用户输入翻译成协议包，发给服务器
            packet = build_packet_from_input(line)
            if packet is None:
                continue
            sock.sendall(encode_packet(packet))
            # quit则退出
            if packet["type"] == "quit":
                break
    except KeyboardInterrupt:
        pass
    finally:
        # 通知接收线程退出，关闭套接字
        stop_event.set()
        try:
            sock.close()
        except OSError:
            pass


if __name__ == "__main__":
    main()
