export function stringToBytes1(str) {
    if (typeof str !== 'string') {
        return str;
    }
    let ch;
    const st8 = [0x08];
    const st16 = [0x10];
    let isUtf16 = 0;
    for (let i = 0; i < str.length; i++) {
        ch = str.charCodeAt(i);
        if (ch > 0x00 && ch < 0x80) {
            st8.push(ch & 0xff);
            st16.push(0x00);
            st16.push(ch & 0xff);
        } else {
            st16.push((ch >> 8) & 0xff);
            st16.push(ch & 0xff);
            isUtf16 = 1;
        }
    }
    return isUtf16 === 1 ? st16 : st8;
}
