/* player client
 * packet.ts
 *
 */

export const dataPacket = {

    KVM_VIDEO_CMD_CONNECT : 0x25,
    KVM_VIDEO_CMD_DISCONNECT : 0x26,
    KVM_VIDEO_MSG_HEARTBEAT : 0x27,
    KVM_VIDEO_CMD_FRAMESPEEN : 0x28,
    KVM_VIDEO_CMD_SENDVIDEO : 0x29,
    KVM_VIDEO_CMD_SUSPEND : 0x30,
    KVM_VIDEO_CMD_CONTINUING : 0x31,
    KVM_VIDEO_CMD_REPLAY : 0x32,
    KVM_VIDEO_CMD_JUMPING : 0x33,
    KVM_VIDEO_CMD_STOP : 0x34,
    KVM_VIDEO_CMD_BEFORRERECON : 0x35,
    KVM_VIDEO_CMD_RECONNECT : 0x36,
    PACKHEAD1 : -2,
    PACKHEAD2 : -10,


    // 封装数据包头
    _MakePacketHeader(packet, length, cmd) {
        packet[0] = dataPacket.PACKHEAD1 & 0xff;
        packet[1] = dataPacket.PACKHEAD2 & 0xff;
        packet[2] = 0x00;
        packet[3] = length & 0xff;

        packet[4] = 0x00;
        packet[5] = 0x00;
        packet[6] = 0x00;
        packet[7] = 0x00;

        packet[8] = 0x00;
        packet[9] = 0x00;

        packet[10] = cmd & 0xff;
        packet[11] = 0x00;

    },

    /*重连前发送的状态命
    *parm videotype:录像类型 playmode:播放模式 state:,speed:,
    *
    */
    beforReConnect(videotype, playmode, state, speed) {
        const self = this;
        const bytes = new Uint8Array(16);

        self._MakePacketHeader(bytes, 0x08, dataPacket.KVM_VIDEO_CMD_BEFORRERECON);

        bytes[12] = videotype & 0xFF;
        bytes[13] = playmode & 0xFF;
        bytes[14] = state & 0xFF;
        bytes[15] = speed & 0xFF;
        return bytes;
    },

    // 重连
    reConnect(seq) {
        const self = this;
        const bytes = new Uint8Array(16);
        self._MakePacketHeader(bytes, 0x08, dataPacket.KVM_VIDEO_CMD_RECONNECT);

        bytes[12 + 3] = seq & 0xFF;
        bytes[12 + 2] = (seq >> 8) & 0xFF;
        bytes[12 + 1] = (seq >> 16) & 0xFF;
        bytes[12]   = (seq >> 24) & 0xFF;

        return bytes;
    },
    // 连接指令获取
    connect(codekey) {
        const self = this;
        const bytes = new Uint8Array(40);
        self._MakePacketHeader(bytes, 0x20, dataPacket.KVM_VIDEO_CMD_CONNECT);
        bytes[12] = 0x00;
        bytes[13] = 0x00;
        bytes[14] = 0x00;
        bytes[15] = 0x00;
        const key = codekey.slice(16, 40);
        for (let i = 0; i < key.length; i++) {
            bytes[16 + i] = key[i] & 0xff;
        }
        return bytes;
    },

    // 断开连接指令
    disConnect() {
        const self = this;
        const bytes = new Uint8Array(16);

        self._MakePacketHeader(bytes, 0x08, dataPacket.KVM_VIDEO_CMD_DISCONNECT);

        bytes[12] = 0x00;
        bytes[13] = 0x00;
        bytes[14] = 0x00;
        bytes[15] = 0x00;
        return bytes;
    },

    // 构建心跳包指令
    heartBeat() {
        const self = this;
        const bytes = new Uint8Array(16);
        self._MakePacketHeader(bytes, 0x08, dataPacket.KVM_VIDEO_MSG_HEARTBEAT);

        bytes[12] = 0x00;
        bytes[13] = 0x00;
        bytes[14] = 0x00;
        bytes[15] = 0x00;
        return bytes;
    },

    // 播放速度指令
    // speed: 00:1倍速,01:0.25倍速, 02:0.5倍速,03:2倍速, 04:4倍速
    speedFrame(speed) {
        const self = this;
        const bytes = new Uint8Array(16);

        self._MakePacketHeader(bytes, 0x08, dataPacket.KVM_VIDEO_CMD_FRAMESPEEN);

        bytes[12] = 0x00;
        bytes[13] = 0x00;
        bytes[14] = 0x00;
        bytes[15] = speed & 0xFF;
        return bytes;
    },

    /*
    *请求发送录像指令
    *parm:videotype:录像类型 0x01:HOST黑屏录像 0x02:CPU严重错误录像 0x03:HOST多状态录像
    *parm:playmode:播放模式 现在仅支持只播放，1；
    */
    sendVideo(videotype, playmode) {
        const self = this;
        const bytes = new Uint8Array(16);
        self._MakePacketHeader(bytes, 0x08, dataPacket.KVM_VIDEO_CMD_SENDVIDEO);
        bytes[12] = 0x00;
        bytes[13] = videotype & 0xFF;
        bytes[14] = 0x00;
        bytes[15] = playmode & 0xFF;
        return bytes;
    },
    // 暂停发送录像指令
    suspend() {
        const self = this;
        const bytes = new Uint8Array(16);
        self._MakePacketHeader(bytes, 0x08, dataPacket.KVM_VIDEO_CMD_SUSPEND);

        bytes[12] = 0x00;
        bytes[13] = 0x00;
        bytes[14] = 0x00;
        bytes[15] = 0x00;
        return bytes;
    },

    // 继续播放指令
    continuing() {
        const self = this;
        const bytes = new Uint8Array(16);
        self._MakePacketHeader(bytes, 0x08, dataPacket.KVM_VIDEO_CMD_CONTINUING);

        bytes[12] = 0x00;
        bytes[13] = 0x00;
        bytes[14] = 0x00;
        bytes[15] = 0x00;
        return bytes;
    },

    // 调帧指令
    jumpingFrame(skippoint) {
        const self = this;
        const bytes = new Uint8Array(16);
        self._MakePacketHeader(bytes, 0x08, dataPacket.KVM_VIDEO_CMD_JUMPING);

        // 将skippoint转换成四个字节 放在12~15中
        bytes[12 + 3] = skippoint & 0xFF;
        bytes[12 + 2] = (skippoint >> 8) & 0xFF;
        bytes[12 + 1] = (skippoint >> 16) & 0xFF;
        bytes[12]   = (skippoint >> 24) & 0xFF;
        return bytes;
    },

    // 停止发送数据录像指令
    stop() {
        const self = this;
        const bytes = new Uint8Array(16);
        self._MakePacketHeader(bytes, 0x08, dataPacket.KVM_VIDEO_CMD_STOP);
        bytes[12] = 0x00;
        bytes[13] = 0x00;
        bytes[14] = 0x00;
        bytes[15] = 0x00;
        return bytes;
    },
};
