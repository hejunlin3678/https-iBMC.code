export class Image2rep {
    public _FILETOP = 0x01;
    public _SEQINDEX = 0x02;
    public _IMAGEDATA = 0x03;
    public _MAX_RECORD_SIZE = 512 * 1024 * 1024;
    public _indexPos = 0;
    public _frameSeqIndex = 0;
    public _indexCount = 20;
    public _fileHead;
    public _fileTotalBody = new Blob([]);
    public _seqIndex;
    public _fileBody = new Blob([]);
    public _fileLength = 52 + 0;
    public _recordState = 0;

    _InitRecord() {
        this._indexPos = 0;
        this._frameSeqIndex = 0;
        this._fileLength = 52 + 0;
        this._fileBody = new Blob([]);
        this._fileTotalBody = new Blob([]);
        this._fileHead = this._fileHeaderData(0, 0);
        this._seqIndex = this._seqIndexData();
    }

    _SaveRecord() {
        if (this._seqIndex != null && this._fileBody != null) {
            this._fileBody = new Blob([this._seqIndex, this._fileBody]);
            this._fileTotalBody = new Blob([this._fileTotalBody, this._fileBody]);
            this._seqIndex = null;
            this._fileBody = new Blob([]);
        }

        const file = new Blob([this._fileHead, this._fileTotalBody]);
        const link = document.createElement('a');
        link.innerHTML = 'downloadVideo';
        const date = new Date();
        link.download =
            'record-' +
            date.getFullYear() +
            '-' +
            (date.getMonth() + 1) +
            '-' +
            date.getDate() +
            '-' +
            date.getHours() +
            ':' +
            date.getMinutes() +
            ':' +
            date.getSeconds() +
            '.rep';
            const myURL = window.URL || window.webkitURL;
            link.href = myURL.createObjectURL(file);
            link.style.visibility = 'hidden';
            const evt = document.createEvent('MouseEvents');
            evt.initEvent('click', true, true);
            link.dispatchEvent(evt);
    }

    startRecord () {
        this._InitRecord();
        this._recordState = 1;
    }

    stopRecord () {
        this._recordState = 0;
        this._SaveRecord();
    }

    writeFrame (width, height, dqt, isIFrame, sourceData) {
        if (this._recordState === 0) {
            return;
        }
        if (this._recordState === 1 && isIFrame !== 0) {
            return;
        }

        this._recordState = 2;
        this._frameSeqIndex++;
        if (isIFrame === 0) {
            if (this._fileTotalBody.size > this._MAX_RECORD_SIZE) {
                this._SaveRecord();
                this._InitRecord();
            }

            if (this._indexPos < this._indexCount) {
                this._updateSeqIndex(this._frameSeqIndex);
            } else {
                this._updateSeqIndexPos(this._fileLength);
                this._fileBody = new Blob([this._seqIndex, this._fileBody]);
                this._seqIndex = null;
                this._fileTotalBody = new Blob([this._fileTotalBody, this._fileBody]);
                this._fileBody = new Blob([]);
                this._addSeqIndex(this._frameSeqIndex);
            }
        }

        this._fileBody = new Blob([
            this._fileBody,
            this._fileImageData(width, height, this._frameSeqIndex, dqt, isIFrame, sourceData)
        ]);

        this._fileHead = this._fileHeaderData(this._frameSeqIndex, this._fileLength);
    }

    _fileHeaderData (totalFrame, fileLength) {
        const datas = new Uint8Array(52);
        datas[0] = 0xfe;
        datas[1] = 0xf6;
        datas[2] = (datas.length >> 24) & 0xff;
        datas[3] = (datas.length >> 16) & 0xff;
        datas[4] = (datas.length >> 8) & 0xff;
        datas[5] = (datas.length >> 0) & 0xff;
        datas[6] = this._FILETOP;
        datas[7] = 0;

        datas[8] = (totalFrame >> 24) & 0xff;
        datas[9] = (totalFrame >> 16) & 0xff;
        datas[10] = (totalFrame >> 8) & 0xff;
        datas[11] = (totalFrame >> 0) & 0xff;

        datas[12] = 0;
        datas[13] = 0;
        datas[14] = 0;
        datas[15] = 0;

        datas[16] = 0;
        datas[17] = 0;
        datas[18] = 0;
        datas[19] = 0;

        const dData = this._rightShift64(fileLength);
        datas[20] = dData[0];
        datas[21] = dData[1];
        datas[22] = dData[2];
        datas[23] = dData[3];
        datas[24] = dData[4];
        datas[25] = dData[5];
        datas[26] = dData[6];
        datas[27] = dData[7];

        datas[28] = 0;
        datas[29] = 4;
        return datas;
    }

    _addSeqIndex (frameNum) {
        this._seqIndex = this._seqIndexData();
        this._indexPos = 0;
        this._updateSeqIndex(frameNum);
    }

    _seqIndexData () {
        const datas = new Uint8Array(7 + this._indexCount * (4 + 8) + 8);
        this._fileLength = this._fileLength + 7 + this._indexCount * (4 + 8) + 8;

        datas[0] = 0xfe;
        datas[1] = 0xf6;

        datas[2] = (datas.length >> 24) & 0xff;
        datas[3] = (datas.length >> 16) & 0xff;
        datas[4] = (datas.length >> 8) & 0xff;
        datas[5] = (datas.length >> 0) & 0xff;

        datas[6] = this._SEQINDEX;

        return datas;
    }

    _updateSeqIndex (frameNum) {
        const iframeIndex = new Uint8Array(12);
        iframeIndex[0] = (frameNum >> 24) & 0xff;
        iframeIndex[1] = (frameNum >> 16) & 0xff;
        iframeIndex[2] = (frameNum >> 8) & 0xff;
        iframeIndex[3] = (frameNum >> 0) & 0xff;

        const dData = this._rightShift64(this._fileLength);
        iframeIndex[4] = dData[0];
        iframeIndex[5] = dData[1];
        iframeIndex[6] = dData[2];
        iframeIndex[7] = dData[3];
        iframeIndex[8] = dData[4];
        iframeIndex[9] = dData[5];
        iframeIndex[10] = dData[6];
        iframeIndex[11] = dData[7];

        this._seqIndex[7 + this._indexPos * 12 + 0] = iframeIndex[0];
        this._seqIndex[7 + this._indexPos * 12 + 1] = iframeIndex[1];
        this._seqIndex[7 + this._indexPos * 12 + 2] = iframeIndex[2];
        this._seqIndex[7 + this._indexPos * 12 + 3] = iframeIndex[3];
        this._seqIndex[7 + this._indexPos * 12 + 4] = iframeIndex[4];
        this._seqIndex[7 + this._indexPos * 12 + 5] = iframeIndex[5];
        this._seqIndex[7 + this._indexPos * 12 + 6] = iframeIndex[6];
        this._seqIndex[7 + this._indexPos * 12 + 7] = iframeIndex[7];
        this._seqIndex[7 + this._indexPos * 12 + 8] = iframeIndex[8];
        this._seqIndex[7 + this._indexPos * 12 + 9] = iframeIndex[9];
        this._seqIndex[7 + this._indexPos * 12 + 10] = iframeIndex[10];
        this._seqIndex[7 + this._indexPos * 12 + 11] = iframeIndex[11];

        this._indexPos = this._indexPos + 1;
    }

    _updateSeqIndexPos(pos) {
        const datas = new Uint8Array(8);
        const dData = this._rightShift64(pos);
        datas[0] = dData[0];
        datas[1] = dData[1];
        datas[2] = dData[2];
        datas[3] = dData[3];
        datas[4] = dData[4];
        datas[5] = dData[5];
        datas[6] = dData[6];
        datas[7] = dData[7];

        this._seqIndex[7 + this._indexPos * 12 + 0] = datas[0];
        this._seqIndex[7 + this._indexPos * 12 + 1] = datas[1];
        this._seqIndex[7 + this._indexPos * 12 + 2] = datas[2];
        this._seqIndex[7 + this._indexPos * 12 + 3] = datas[3];
        this._seqIndex[7 + this._indexPos * 12 + 4] = datas[4];
        this._seqIndex[7 + this._indexPos * 12 + 5] = datas[5];
        this._seqIndex[7 + this._indexPos * 12 + 6] = datas[6];
        this._seqIndex[7 + this._indexPos * 12 + 7] = datas[7];
    }

    _fileImageData(width, height, seqFrame, dqt, isIFrame, sourceData) {
        const imageData = new Uint8Array(sourceData.length + 25);
        this._fileLength = this._fileLength + sourceData.length + 25;
        imageData[0] = 0xfe;
        imageData[1] = 0xf6;
        imageData[2] = (imageData.length >> 24) & 0xff;
        imageData[3] = (imageData.length >> 16) & 0xff;
        imageData[4] = (imageData.length >> 8) & 0xff;
        imageData[5] = (imageData.length >> 0) & 0xff;
        imageData[6] = this._IMAGEDATA;
        imageData[7] = ((dqt & 0xff) << 4) | (isIFrame & 0xff);
        imageData[8] = 0;
        imageData[9] = (seqFrame >> 24) & 0xff;
        imageData[10] = (seqFrame >> 16) & 0xff;
        imageData[11] = (seqFrame >> 8) & 0xff;
        imageData[12] = (seqFrame >> 0) & 0xff;
        imageData[13] = (width >> 8) & 0xff;
        imageData[14] = (width >> 0) & 0xff;
        imageData[15] = (height >> 8) & 0xff;
        imageData[16] = (height >> 0) & 0xff;

        const nowTime = new Date().getTime();
        const dData = this._rightShift64(nowTime);
        imageData[17] = dData[0];
        imageData[18] = dData[1];
        imageData[19] = dData[2];
        imageData[20] = dData[3];
        imageData[21] = dData[4];
        imageData[22] = dData[5];
        imageData[23] = dData[6];
        imageData[24] = dData[7];

        for (let i = 0; i < sourceData.length; i++) {
            imageData[i + 25] = sourceData[i] & 0xff;
        }
        return imageData;
    }

    _rightShift64(sData) {
        const dData = new Uint8Array(8);
        const temp = sData.toString(16);
        const myLength = 16 - temp.length;
        let temp1 = '';
        for (let i = 0; i < myLength; i++) {
            temp1 += '0';
        }
        temp1 += temp;
        dData[0] = Number('0x' + temp1.substring(0, 2)) & 0xff;
        dData[1] = Number('0x' + temp1.substring(2, 4)) & 0xff;
        dData[2] = Number('0x' + temp1.substring(4, 6)) & 0xff;
        dData[3] = Number('0x' + temp1.substring(6, 8)) & 0xff;
        dData[4] = Number('0x' + temp1.substring(8, 10)) & 0xff;
        dData[5] = Number('0x' + temp1.substring(10, 12)) & 0xff;
        dData[6] = Number('0x' + temp1.substring(12, 14)) & 0xff;
        dData[7] = Number('0x' + temp1.substring(14, 16)) & 0xff;
        return dData;
    }
}
