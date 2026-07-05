# 多线程并发聊天室

基于 Python 实现的 TCP 多线程聊天室，支持群聊、私聊、在线列表和上下线通知
应用层使用 JSON Lines 协议，并有效处理 TCP 的粘包/半包问题

## 目录结构

```text
src/
├─ chat_server.py     # 多线程聊天室服务器
├─ chat_client.py     # 命令行客户端
└─ chat_protocol.py   # JSON Lines 协议编解码
```

## 启动服务端

```powershell
python src\chat_server.py --host 127.0.0.1 --port 9009
```

`--host` 和 `--port` 可省略，默认监听 `0.0.0.0:9009`

看到 `Chat server listening on ...` 即表示启动成功。

## 启动客户端

另开多个终端运行：

```powershell
python src\chat_client.py --host 127.0.0.1 --port 9009
```

参数需与服务端一致，`--host` 和 `--port` 为默认时可省略

## 使用方法

连接后先用 `/login` 登录，再发消息。支持以下命令：

| 命令 | 说明 |
| --- | --- |
| `/login 名字` | 登录并设置用户名（不可重名、不含空格） |
| 直接输入文字 | 群聊，广播给所有在线用户 |
| `/w 名字 内容` | 私聊，只发给指定用户 |
| `/list` | 查看当前在线用户 |
| `/quit` | 退出 |
