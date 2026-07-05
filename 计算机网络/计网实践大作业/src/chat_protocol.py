"""聊天室应用层协议：JSON Lines 的编码与解码

一条消息 =  JSON 对象 + '\n'，
用换行符作为帧的分隔符，每个包需要带 "type" 字段标明用途
"""

import json

class ProtocolError(Exception):
    """协议解析失败时抛出"""


def encode_packet(packet):
    """把字典编码成一行可直接发送的字节"""
    # 协议要求每个包必须是 JSON 对象，且必须带非空 "type" 字段
    if not isinstance(packet, dict):
        raise TypeError("packet must be a dict")
    if not isinstance(packet.get("type"), str) or not packet["type"]:
        raise ProtocolError("packet type is required")
    # ensure_ascii=False 允许输出非 ASCII 字符，separators=(",", ":") 去掉默认的空格，生成更紧凑的 JSON 文本
    text = json.dumps(packet, ensure_ascii=False, separators=(",", ":")) + "\n"
    return text.encode("utf-8")


class JsonLineParser:
    """流式解析器，把 TCP 字节流还原成一个个完整数据包"""

    def __init__(self, max_line_length=65536):
        # max_line_length 限制单包大小，防止内存耗尽
        self.buffer = bytearray()
        self.max_line_length = max_line_length

    def feed(self, data):
        """输入一段字节，返回其中所有完整的数据包"""
        # 与上次残留的半包拼接
        self.buffer.extend(data)
        if len(self.buffer) > self.max_line_length:
            raise ProtocolError("packet is too large")

        packets = []
        # 一次 feed 可能含多条消息（粘包），循环取出所有完整的行
        while True:
            newline_index = self.buffer.find(b"\n")
            if newline_index == -1:
                # 剩下的不足一行（半包），留在缓冲区等后续数据
                break

            # 取出一行；rstrip(b"\r") 兼容 Windows 的 "\r\n"
            line = bytes(self.buffer[:newline_index]).rstrip(b"\r")
            del self.buffer[: newline_index + 1]
            if not line:
                continue

            try:
                # 把 JSON 文本解码成 Python 对象
                packet = json.loads(line.decode("utf-8"))
            except (UnicodeDecodeError, json.JSONDecodeError) as exc:
                raise ProtocolError("invalid json packet") from exc

            # 协议要求每个包是带非空 "type" 的 JSON 对象，否则无法分派
            if not isinstance(packet, dict):
                raise ProtocolError("packet must be a json object")
            if not isinstance(packet.get("type"), str) or not packet["type"]:
                raise ProtocolError("packet type is required")
            
            # 解析成功，加入结果列表，继续解析下一行
            packets.append(packet)
            
        # 返回所有解析成功的包
        return packets
