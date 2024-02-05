import { UdfImageBuilderFile } from './udfImageBuilderFile';
import { UdfLayoutInformation } from './udfLayoutInformation';
import { stringToBytes1 } from '../kvm-h5.util';

export class UdfImageBuilder {
    public imageIdentifier = 'SabreUDFImageBuilder Disc';
    public applicationIdentifier = '*SabreUDFImageBuilder';
    public applicationIdentifierSuffix = [0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00];
    public blockSize = 2048;
    public udfVersionIdentifierSuffix = [0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00];
    public rootUDFImageBuilderFile;
    public version = 'Revision102';
    public minimumUDFReadRevision = 0x0102;
    public minimumUDFWriteRevision = 0x0102;
    public maximumUDFWriteRevision = 0x0102;
    public maximumAllocationLength = 1073739776;
    public outBlob;
    public allSmallFile = 0;
    public currentSmalFile = 0;
    public udfLayoutInformation;
    public fileBlob = null;
    public _reader = null;
    public udfImageFile;

    isISOReady() {
        if (this.fileBlob != null) {
            return true;
        }
        return false;
    }

    creatUdfImageBuilderFile(fileList) {
        const rootUdfImageBuilderFile = new UdfImageBuilderFile();
        rootUdfImageBuilderFile.identifier = '';
        rootUdfImageBuilderFile.sourceFile = null;
        rootUdfImageBuilderFile.fileType = 'Directory';

        rootUdfImageBuilderFile.children = [];
        rootUdfImageBuilderFile.accessTime = new Date().getTime();
        rootUdfImageBuilderFile.attributeTime = new Date().getTime();
        rootUdfImageBuilderFile.creationTime = new Date().getTime();
        rootUdfImageBuilderFile.modificationTIme = new Date().getTime();
        rootUdfImageBuilderFile.fileLinkCount = 1;
        rootUdfImageBuilderFile.parent = null;
        for (const file of fileList) {
            const path = file.webkitRelativePath || file.name;
            const nameArray = path.split('/');
            let currentParent = rootUdfImageBuilderFile;
            for (let j = 1; j < nameArray.length - 1; j++) {
                let flag = true;
                for (const children of currentParent.children) {
                    if (nameArray[j] === children.identifier) {
                        flag = false;
                        currentParent = children;
                        break;
                    }
                }
                if (flag) {
                    const myUdfFile = new UdfImageBuilderFile();
                    myUdfFile.identifier = nameArray[j];
                    myUdfFile.sourceFile = null;
                    myUdfFile.fileType = 'Directory';
                    myUdfFile.children = [];
                    myUdfFile.accessTime = new Date().getTime();
                    myUdfFile.attributeTime = file.lastModified;
                    myUdfFile.creationTime = file.lastModified;
                    myUdfFile.modificationTIme = file.lastModified;
                    myUdfFile.fileLinkCount = 1;
                    myUdfFile.parent = currentParent;
                    currentParent.children.push(myUdfFile);
                    currentParent.fileLinkCount++;
                    currentParent = myUdfFile;
                }
            }
            const myUdfFile2 = new UdfImageBuilderFile();
            myUdfFile2.identifier = nameArray[nameArray.length - 1];
            myUdfFile2.sourceFile = new Blob([file.slice(0, file.size)], {
                type: 'application/txt'
            });
            myUdfFile2.fileType = 'File';
            myUdfFile2.children = null;
            myUdfFile2.accessTime = new Date().getTime();
            myUdfFile2.attributeTime = file.lastModified;
            myUdfFile2.creationTime = file.lastModified;
            myUdfFile2.modificationTIme = file.lastModified;
            myUdfFile2.fileLinkCount = 1;
            myUdfFile2.parent = currentParent;
            currentParent.children.push(myUdfFile2);
        }
        return rootUdfImageBuilderFile;
    }

    setSourceFiles(sourceFiles) {
        const rootUdfImageBuilderFile = this.creatUdfImageBuilderFile(sourceFiles);
        this.rootUDFImageBuilderFile = rootUdfImageBuilderFile;
    }
    setImageIdentifier(id) {
        this.imageIdentifier = id;
    }
    execute() {
        const myUDFLayoutInformation = new UdfLayoutInformation();
        myUDFLayoutInformation.init(this.rootUDFImageBuilderFile, this.version, this.blockSize);
        this.udfLayoutInformation = myUDFLayoutInformation;
        this.allSmallFile = 0;
        this.currentSmalFile = 0;
        for (const udfImageFile of myUDFLayoutInformation.linearUDFImageBuilderFileOrdering) {
            if (udfImageFile.fileType === 'File' && udfImageFile.getFileLength() <= 2048 - 176) {
                this.allSmallFile++;
            }
        }
        if (this.allSmallFile === 0) {
            this.fileBlob = this.writeImage();
        }
        for (const udfImageFile2 of myUDFLayoutInformation.linearUDFImageBuilderFileOrdering) {
            if (udfImageFile2.fileType === 'File' && udfImageFile2.getFileLength() <= 2048 - 176) {
                this._reader = new FileReader();
                this._reader.onloadend = (e) => {
                    this._reader.udfImageFile2.sourceFileArray = new Uint8Array(e.target.result);
                    this.currentSmalFile++;
                    if (this.currentSmalFile === this.allSmallFile) {
                        this.fileBlob = this.writeImage();
                    }
                };
                this._reader.udfImageFile2 = udfImageFile2;
                this._reader.readAsArrayBuffer(udfImageFile2.sourceFile);
            } else {
                udfImageFile2.sourceFileArray = null;
            }
        }
    }
    writeImage() {
        const recordingTimeMillis = new Date().getTime();
        const myUDFLayoutInformation = this.udfLayoutInformation;

        this.outBlob = new Blob([], {
            type: 'application/txt'
        });

        const ra = new Uint8Array(16 * 2048);
        this.outBlob = new Blob([this.outBlob, ra]);

        this.writeVRS(myUDFLayoutInformation);

        const emptyArea1 = new Uint8Array(237 * 2048);
        this.outBlob = new Blob([this.outBlob, emptyArea1]);

        this.writeAVDP(
            myUDFLayoutInformation,
            myUDFLayoutInformation.avdp1Block,
            myUDFLayoutInformation.mvdsStartingBlock,
            myUDFLayoutInformation.rvdsStartingBlock
        );

        this.writePVD(myUDFLayoutInformation, myUDFLayoutInformation.pvd1Block, recordingTimeMillis);

        this.writePD(myUDFLayoutInformation, myUDFLayoutInformation.pd1Block);

        this.writeLVD(myUDFLayoutInformation, myUDFLayoutInformation.lvd1Block);

        this.writeUSD(myUDFLayoutInformation, myUDFLayoutInformation.usd1Block);

        this.writeIUVD(myUDFLayoutInformation, myUDFLayoutInformation.iuvd1Block);

        this.writeTD(myUDFLayoutInformation, myUDFLayoutInformation.td1Block);

        const emptyArea2 = new Uint8Array(10 * 2048);
        this.outBlob = new Blob([this.outBlob, emptyArea2]);

        this.writeLVID(myUDFLayoutInformation, recordingTimeMillis);

        this.writeTD(myUDFLayoutInformation, myUDFLayoutInformation.lvidsStartingBlock + 1);

        const emptyArea3 = new Uint8Array(2 * 2048);
        this.outBlob = new Blob([this.outBlob, emptyArea3]);

        const emptyArea4 = new Uint8Array(1 * 2048);
        this.outBlob = new Blob([this.outBlob, emptyArea4]);

        this.writeFSD(myUDFLayoutInformation, recordingTimeMillis);

        for (const udfImageFile of myUDFLayoutInformation.linearUDFImageBuilderFileOrdering) {
            const myFileEntyPosition = udfImageFile.fileEntryPosition;
            if (udfImageFile.fileType === 'File') {
                const fe = new Uint8Array(1 * 2048);
                const filelength = udfImageFile.getFileLength();
                let lAd = 0;
                let icbTagFlags;
                let logicalBlocksRecorded;
                if (filelength <= 2048 - 176) {
                    lAd = filelength;
                    icbTagFlags = 3;
                    logicalBlocksRecorded = 0;
                } else {
                    let restFileSize = filelength;
                    icbTagFlags = 1;
                    logicalBlocksRecorded = Math.ceil(filelength / 2048);
                    while (restFileSize > 0) {
                        lAd += 16;
                        restFileSize -= this.maximumAllocationLength;
                    }
                }
                const feBody = new Uint8Array(160 + lAd);
                feBody[0] = (0 >> 0) & 0xff;
                feBody[1] = (0 >> 8) & 0xff;
                feBody[2] = (0 >> 16) & 0xff;
                feBody[3] = (0 >> 24) & 0xff;

                feBody[4] = (4 >> 0) & 0xff;
                feBody[5] = (4 >> 8) & 0xff;

                feBody[8] = (1 >> 0) & 0xff;
                feBody[9] = (1 >> 8) & 0xff;

                feBody[11] = (5 >> 0) & 0xff;

                feBody[18] = (icbTagFlags >> 0) & 0xff;
                feBody[19] = (icbTagFlags >> 8) & 0xff;

                feBody[20] = 0xff;
                feBody[21] = 0xff;
                feBody[22] = 0xff;
                feBody[23] = 0xff;

                feBody[24] = 0xff;
                feBody[25] = 0xff;
                feBody[26] = 0xff;
                feBody[27] = 0xff;

                feBody[28] = (4228 >> 0) & 0xff;
                feBody[29] = (4228 >> 8) & 0xff;
                feBody[30] = (4228 >> 16) & 0xff;
                feBody[31] = (4228 >> 24) & 0xff;

                feBody[32] = (udfImageFile.fileLinkCount >> 0) & 0xff;
                feBody[33] = (udfImageFile.fileLinkCount >> 8) & 0xff;

                feBody[34] = 0x00;

                feBody[35] = 0x00;

                feBody[36] = 0x00;
                feBody[37] = 0x00;
                feBody[38] = 0x00;
                feBody[39] = 0x00;

                const informationLength = rightShift64(filelength);
                for (let i = 0; i < 8; i++) {
                    feBody[i + 40] = informationLength[i] & 0xff;
                }

                const logicalBlocksRecordedUint64 = rightShift64(logicalBlocksRecorded);
                for (let j = 0; j < 8; j++) {
                    feBody[j + 48] = logicalBlocksRecordedUint64[j];
                }

                const accessTime = millis2TimeStamp(udfImageFile.accessTime);
                for (let k = 0; k < 12; k++) {
                    feBody[k + 56] = accessTime[k] & 0xff;
                }

                const modificationTime = millis2TimeStamp(udfImageFile.modificationTIme);
                for (let o = 0; o < 12; o++) {
                    feBody[o + 68] = modificationTime[o] & 0xff;
                }

                const attributeTime = millis2TimeStamp(udfImageFile.attributeTime);
                for (let u = 0; u < 12; u++) {
                    feBody[u + 80] = attributeTime[u] & 0xff;
                }

                feBody[92] = (1 >> 0) & 0xff;
                feBody[93] = (1 >> 8) & 0xff;
                feBody[94] = (1 >> 16) & 0xff;
                feBody[95] = (1 >> 24) & 0xff;

                feBody[112] = 0x00;
                const applicationIdentifier = stringToBytes(this.applicationIdentifier);
                for (let p = 0; p < applicationIdentifier.length; p++) {
                    feBody[p + 113] = applicationIdentifier[p] & 0xff;
                }
                for (let q = 0; q < 8; q++) {
                    feBody[q + 136] = this.applicationIdentifierSuffix[q];
                }

                const uniqueID = rightShift64(myFileEntyPosition.uniqueIds);
                for (let r = 0; r < 8; r++) {
                    feBody[r + 144] = uniqueID[r] & 0xff;
                }

                feBody[156] = (lAd >> 0) & 0xff;
                feBody[157] = (lAd >> 8) & 0xff;
                feBody[158] = (lAd >> 16) & 0xff;
                feBody[159] = (lAd >> 24) & 0xff;

                if (filelength <= 2048 - 176) {
                    if (udfImageFile.sourceFile != null) {
                        for (let y = 0; y < udfImageFile.sourceFileArray.length; y++) {
                            feBody[y + 160] = udfImageFile.sourceFileArray[y];
                        }
                    }
                } else {
                    let currentExtentPosition = myFileEntyPosition.dataLocation;
                    let restFileSize2 = filelength;
                    let l = 0;
                    while (restFileSize2 > 0) {
                        const extentLength =
                            restFileSize2 < this.maximumAllocationLength ? restFileSize2 : this.maximumAllocationLength;
                        feBody[160 + l * 16 + 0] = (extentLength >> 0) & 0xff;
                        feBody[160 + l * 16 + 1] = (extentLength >> 8) & 0xff;
                        feBody[160 + l * 16 + 2] = (extentLength >> 16) & 0xff;
                        feBody[160 + l * 16 + 3] = (extentLength >> 24) & 0xff;

                        feBody[160 + l * 16 + 4] = (currentExtentPosition >> 0) & 0xff;
                        feBody[160 + l * 16 + 5] = (currentExtentPosition >> 8) & 0xff;
                        feBody[160 + l * 16 + 6] = (currentExtentPosition >> 16) & 0xff;
                        feBody[160 + l * 16 + 7] = (currentExtentPosition >> 24) & 0xff;
                        l++;
                        restFileSize2 -= this.maximumAllocationLength;
                        currentExtentPosition += Math.ceil(this.maximumAllocationLength / 2048);
                    }
                }
                const descriptorCRC = checkSum(feBody);
                fe[0] = (261 >> 0) & 0xff;
                fe[1] = (261 >> 8) & 0xff;
                fe[2] = (2 >> 0) & 0xff;
                fe[3] = (2 >> 8) & 0xff;
                fe[6] = (1 >> 0) & 0xff;
                fe[7] = (1 >> 8) & 0xff;
                fe[8] = (descriptorCRC >> 0) & 0xff;
                fe[9] = (descriptorCRC >> 8) & 0xff;
                fe[10] = ((160 + lAd) >> 0) & 0xff;
                fe[11] = ((160 + lAd) >> 8) & 0xff;
                fe[12] = (myFileEntyPosition.entryLocation >> 0) & 0xff;
                fe[13] = (myFileEntyPosition.entryLocation >> 8) & 0xff;
                fe[14] = (myFileEntyPosition.entryLocation >> 16) & 0xff;
                fe[15] = (myFileEntyPosition.entryLocation >> 24) & 0xff;
                const checksum =
                    (fe[0] +
                        fe[1] +
                        fe[2] +
                        fe[3] +
                        fe[5] +
                        fe[6] +
                        fe[7] +
                        fe[8] +
                        fe[9] +
                        fe[10] +
                        fe[11] +
                        fe[12] +
                        fe[13] +
                        fe[14] +
                        fe[15]) &
                    0xff;
                fe[4] = checksum & 0xff;
                for (let b = 0; b < feBody.length; b++) {
                    fe[b + 16] = feBody[b];
                }
                this.outBlob = new Blob([this.outBlob, fe]);
            } else {
                const fe2 = new Uint8Array(1 * 2048);
                let fidlength = 40;

                const childUDFImageBuilderFiles = udfImageFile.getChildren();
                for (const builderFiles of childUDFImageBuilderFiles) {
                    const lengthWithoutPadding = 38 + stringToBytes1(builderFiles.identifier).length;
                    const paddingLength = lengthWithoutPadding % 4 === 0 ? 0 : 4 - (lengthWithoutPadding % 4);
                    fidlength += lengthWithoutPadding + paddingLength;
                }

                let lAd2 = 0;
                let icbTagFlags2;
                let logicalBlocksRecorded2;
                if (fidlength <= 2048 - 176) {
                    lAd2 = fidlength;
                    icbTagFlags2 = 3;
                    logicalBlocksRecorded2 = 0;
                } else {
                    icbTagFlags2 = 0;
                    logicalBlocksRecorded2 = Math.ceil(fidlength / 2048);
                    lAd2 = 8;
                }
                const feBody2 = new Uint8Array(160 + lAd2);

                feBody2[0] = (0 >> 0) & 0xff;
                feBody2[1] = (0 >> 8) & 0xff;
                feBody2[2] = (0 >> 16) & 0xff;
                feBody2[3] = (0 >> 24) & 0xff;

                feBody2[4] = (4 >> 0) & 0xff;
                feBody2[5] = (4 >> 8) & 0xff;

                feBody2[8] = (1 >> 0) & 0xff;
                feBody2[9] = (1 >> 8) & 0xff;

                feBody2[11] = (4 >> 0) & 0xff;

                feBody2[18] = (icbTagFlags2 >> 0) & 0xff;
                feBody2[19] = (icbTagFlags2 >> 8) & 0xff;

                feBody2[20] = 0xff;
                feBody2[21] = 0xff;
                feBody2[22] = 0xff;
                feBody2[23] = 0xff;

                feBody2[24] = 0xff;
                feBody2[25] = 0xff;
                feBody2[26] = 0xff;
                feBody2[27] = 0xff;

                feBody2[28] = (5285 >> 0) & 0xff;
                feBody2[29] = (5285 >> 8) & 0xff;
                feBody2[30] = (5285 >> 16) & 0xff;
                feBody2[31] = (5285 >> 24) & 0xff;

                feBody2[32] = (udfImageFile.fileLinkCount >> 0) & 0xff;
                feBody2[33] = (udfImageFile.fileLinkCount >> 8) & 0xff;

                feBody2[34] = 0x00;

                feBody2[35] = 0x00;

                feBody2[36] = 0x00;
                feBody2[37] = 0x00;
                feBody2[38] = 0x00;
                feBody2[39] = 0x00;
                const informationLength2 = rightShift64(fidlength);
                for (let h = 0; h < 8; h++) {
                    feBody2[h + 40] = informationLength2[h] & 0xff;
                }
                const logicalBlocksRecordUint64 = rightShift64(logicalBlocksRecorded2);
                for (let t = 0; t < 8; t++) {
                    feBody2[t + 48] = logicalBlocksRecordUint64[t] & 0xff;
                }

                const accessTime2 = millis2TimeStamp(udfImageFile.accessTime);
                for (let x = 0; x < 12; x++) {
                    feBody2[x + 56] = accessTime2[x] & 0xff;
                }

                const modificationTime2 = millis2TimeStamp(udfImageFile.modificationTIme);
                for (let n = 0; n < 12; n++) {
                    feBody2[n + 68] = modificationTime2[n] & 0xff;
                }

                const attributeTime2 = millis2TimeStamp(udfImageFile.attributeTime);
                for (let lp = 0; lp < 12; lp++) {
                    feBody2[lp + 80] = attributeTime2[lp] & 0xff;
                }

                feBody2[92] = (1 >> 0) & 0xff;
                feBody2[93] = (1 >> 8) & 0xff;
                feBody2[94] = (1 >> 16) & 0xff;
                feBody2[95] = (1 >> 24) & 0xff;

                feBody2[112] = 0x00;
                const applicationIdentifier2 = stringToBytes(this.applicationIdentifier);
                for (let bx = 0; bx < applicationIdentifier2.length; bx++) {
                    feBody2[bx + 113] = applicationIdentifier2[bx] & 0xff;
                }
                for (let vc = 0; vc < 8; vc++) {
                    feBody2[vc + 136] = this.applicationIdentifierSuffix[vc];
                }

                const uniqueID2 = rightShift64(myFileEntyPosition.uniqueIds);
                for (let kx = 0; kx < 8; kx++) {
                    feBody2[kx + 144] = uniqueID2[kx] & 0xff;
                }

                feBody2[156] = (lAd2 >> 0) & 0xff;
                feBody2[157] = (lAd2 >> 8) & 0xff;
                feBody2[158] = (lAd2 >> 16) & 0xff;
                feBody2[159] = (lAd2 >> 24) & 0xff;

                const fids = new Uint8Array(fidlength);

                const pFID = new Uint8Array(40);
                const pFIDBody = new Uint8Array(24);

                pFIDBody[0] = (1 >> 0) & 0xff;
                pFIDBody[1] = (1 >> 8) & 0xff;
                pFIDBody[2] = (10 >> 0) & 0xff;

                pFIDBody[4] = (2048 >> 0) & 0xff;
                pFIDBody[5] = (2048 >> 8) & 0xff;
                pFIDBody[6] = (2048 >> 16) & 0xff;
                pFIDBody[7] = (2048 >> 24) & 0xff;

                let parentDirectoryLocation = myFileEntyPosition.entryLocation;
                let parentDirectoryUniqueId = 0;
                if (udfImageFile.parent !== undefined && udfImageFile.parent != null) {
                    parentDirectoryLocation = udfImageFile.parent.fileEntryPosition.entryLocation;
                    parentDirectoryUniqueId = udfImageFile.parent.fileEntryPosition.uniqueIds;
                }
                pFIDBody[8] = (parentDirectoryLocation >> 0) & 0xff;
                pFIDBody[9] = (parentDirectoryLocation >> 8) & 0xff;
                pFIDBody[10] = (parentDirectoryLocation >> 16) & 0xff;
                pFIDBody[11] = (parentDirectoryLocation >> 24) & 0xff;

                pFIDBody[12] = (0 >> 0) & 0xff;
                pFIDBody[13] = (0 >> 8) & 0xff;

                pFIDBody[16] = (parentDirectoryUniqueId >> 0) & 0xff;
                pFIDBody[17] = (parentDirectoryUniqueId >> 8) & 0xff;
                pFIDBody[18] = (parentDirectoryUniqueId >> 16) & 0xff;
                pFIDBody[19] = (parentDirectoryUniqueId >> 24) & 0xff;
                const descriptorCRC4 = checkSum(pFIDBody);

                pFID[0] = (257 >> 0) & 0xff;
                pFID[1] = (257 >> 8) & 0xff;

                pFID[2] = (2 >> 0) & 0xff;
                pFID[3] = (2 >> 8) & 0xff;

                pFID[5] = 0x00;

                pFID[6] = (1 >> 0) & 0xff;
                pFID[7] = (1 >> 8) & 0xff;

                pFID[8] = (descriptorCRC4 >> 0) & 0xff;
                pFID[9] = (descriptorCRC4 >> 8) & 0xff;

                pFID[10] = (24 >> 0) & 0xff;
                pFID[11] = (24 >> 8) & 0xff;

                pFID[12] = (myFileEntyPosition.entryLocation >> 0) & 0xff;
                pFID[13] = (myFileEntyPosition.entryLocation >> 8) & 0xff;
                pFID[14] = (myFileEntyPosition.entryLocation >> 16) & 0xff;
                pFID[15] = (myFileEntyPosition.entryLocation >> 24) & 0xff;
                const checksum4 =
                    (pFID[0] +
                        pFID[1] +
                        pFID[2] +
                        pFID[3] +
                        pFID[5] +
                        pFID[6] +
                        pFID[7] +
                        pFID[8] +
                        pFID[9] +
                        pFID[10] +
                        pFID[11] +
                        pFID[12] +
                        pFID[13] +
                        pFID[14] +
                        pFID[15]) &
                    0xff;

                pFID[4] = checksum4 & 0xff;
                for (let xt = 0; xt < pFIDBody.length; xt++) {
                    pFID[xt + 16] = pFIDBody[xt];
                }
                for (let a = 0; a < 40; a++) {
                    fids[a] = pFID[a];
                }

                let fidPoint = 40;
                const everyFidStartPoint = new Array(udfImageFile.getChildren().length + 1);
                everyFidStartPoint[0] = 0;
                everyFidStartPoint[1] = 40;
                for (let kb = 0; kb < udfImageFile.getChildren().length; kb++) {
                    const cudfImageFile = udfImageFile.getChildren()[kb];
                    const cidentifier = cudfImageFile.identifier;

                    const lengthWithoutPadding2 = 38 + stringToBytes1(cidentifier).length;
                    const paddingLength2 = lengthWithoutPadding2 % 4 === 0 ? 0 : 4 - (lengthWithoutPadding2 % 4);
                    const cfidlength = lengthWithoutPadding2 + paddingLength2;

                    const cFID = new Uint8Array(cfidlength);
                    const cFIDBody = new Uint8Array(cfidlength - 16);
                    cFIDBody[0] = (1 >> 0) & 0xff;
                    cFIDBody[1] = (1 >> 8) & 0xff;
                    if (cudfImageFile.fileType === 'Directory') {
                        cFIDBody[2] = (2 >> 0) & 0xff;
                    } else {
                        cFIDBody[2] = (0 >> 0) & 0xff;
                    }
                    cFIDBody[3] = (stringToBytes1(cidentifier).length >> 0) & 0xff;
                    cFIDBody[4] = (2048 >> 0) & 0xff;
                    cFIDBody[5] = (2048 >> 8) & 0xff;
                    cFIDBody[6] = (2048 >> 16) & 0xff;
                    cFIDBody[7] = (2048 >> 24) & 0xff;
                    const cfileEntyPosition = cudfImageFile.fileEntryPosition;

                    cFIDBody[8] = (cfileEntyPosition.entryLocation >> 0) & 0xff;
                    cFIDBody[9] = (cfileEntyPosition.entryLocation >> 8) & 0xff;
                    cFIDBody[10] = (cfileEntyPosition.entryLocation >> 16) & 0xff;
                    cFIDBody[11] = (cfileEntyPosition.entryLocation >> 24) & 0xff;
                    cFIDBody[12] = (0 >> 0) & 0xff;
                    cFIDBody[13] = (0 >> 8) & 0xff;

                    cFIDBody[16] = (cfileEntyPosition.uniqueIds >> 0) & 0xff;
                    cFIDBody[17] = (cfileEntyPosition.uniqueIds >> 8) & 0xff;
                    cFIDBody[18] = (cfileEntyPosition.uniqueIds >> 16) & 0xff;
                    cFIDBody[19] = (cfileEntyPosition.uniqueIds >> 24) & 0xff;

                    const cFileIdentifier = stringToBytes1(cidentifier);
                    for (let iv = 0; iv < cFileIdentifier.length; iv++) {
                        cFIDBody[22 + iv] = cFileIdentifier[iv];
                    }
                    const descriptorCRC5 = checkSum(cFIDBody);

                    cFID[0] = (257 >> 0) & 0xff;
                    cFID[1] = (257 >> 8) & 0xff;

                    cFID[2] = (2 >> 0) & 0xff;
                    cFID[3] = (2 >> 8) & 0xff;

                    cFID[5] = 0x00;
                    cFID[6] = (1 >> 0) & 0xff;
                    cFID[7] = (1 >> 8) & 0xff;
                    cFID[8] = (descriptorCRC5 >> 0) & 0xff;
                    cFID[9] = (descriptorCRC5 >> 8) & 0xff;
                    cFID[10] = ((cfidlength - 16) >> 0) & 0xff;
                    cFID[11] = ((cfidlength - 16) >> 8) & 0xff;
                    cFID[12] = (myFileEntyPosition.entryLocation >> 0) & 0xff;
                    cFID[13] = (myFileEntyPosition.entryLocation >> 8) & 0xff;
                    cFID[14] = (myFileEntyPosition.entryLocation >> 16) & 0xff;
                    cFID[15] = (myFileEntyPosition.entryLocation >> 24) & 0xff;
                    const checksum5 =
                        (cFID[0] +
                            cFID[1] +
                            cFID[2] +
                            cFID[3] +
                            cFID[5] +
                            cFID[6] +
                            cFID[7] +
                            cFID[8] +
                            cFID[9] +
                            cFID[10] +
                            cFID[11] +
                            cFID[12] +
                            cFID[13] +
                            cFID[14] +
                            cFID[15]) &
                        0xff;
                    cFID[4] = checksum5 & 0xff;
                    for (let cx = 0; cx < cFIDBody.length; cx++) {
                        cFID[cx + 16] = cFIDBody[cx];
                    }
                    for (let ax = 0; ax < cfidlength; ax++) {
                        fids[fidPoint++] = cFID[ax];
                    }
                    if (kb < udfImageFile.getChildren().length - 1) {
                        everyFidStartPoint[kb + 2] = fidPoint;
                    }
                }

                if (fidlength <= 2048 - 176) {
                    for (let hx = 0; hx < fids.length; hx++) {
                        feBody2[hx + 160] = fids[hx];
                    }
                    const descriptorCRC2 = checkSum(feBody2);
                    fe2[0] = (261 >> 0) & 0xff;
                    fe2[1] = (261 >> 8) & 0xff;
                    fe2[2] = (2 >> 0) & 0xff;
                    fe2[3] = (2 >> 8) & 0xff;
                    fe2[5] = 0x00;
                    fe2[6] = (1 >> 0) & 0xff;
                    fe2[7] = (1 >> 8) & 0xff;
                    fe2[8] = (descriptorCRC2 >> 0) & 0xff;
                    fe2[9] = (descriptorCRC2 >> 8) & 0xff;
                    fe2[10] = ((160 + lAd2) >> 0) & 0xff;
                    fe2[11] = ((160 + lAd2) >> 8) & 0xff;
                    fe2[12] = (myFileEntyPosition.entryLocation >> 0) & 0xff;
                    fe2[13] = (myFileEntyPosition.entryLocation >> 8) & 0xff;
                    fe2[14] = (myFileEntyPosition.entryLocation >> 16) & 0xff;
                    fe2[15] = (myFileEntyPosition.entryLocation >> 24) & 0xff;
                    const checksum2 =
                        (fe2[0] +
                            fe2[1] +
                            fe2[2] +
                            fe2[3] +
                            fe2[5] +
                            fe2[6] +
                            fe2[7] +
                            fe2[8] +
                            fe2[9] +
                            fe2[10] +
                            fe2[11] +
                            fe2[12] +
                            fe2[13] +
                            fe2[14] +
                            fe2[15]) &
                        0xff;
                    fe2[4] = checksum2 & 0xff;
                    for (let fx = 0; fx < feBody2.length; fx++) {
                        fe2[fx + 16] = feBody2[fx];
                    }
                    this.outBlob = new Blob([this.outBlob, fe2]);
                } else {
                    feBody2[160] = (fidlength >> 0) & 0xff;
                    feBody2[161] = (fidlength >> 8) & 0xff;
                    feBody2[162] = (fidlength >> 16) & 0xff;
                    feBody2[163] = (fidlength >> 24) & 0xff;

                    feBody2[164] = (myFileEntyPosition.dataLocation >> 0) & 0xff;
                    feBody2[165] = (myFileEntyPosition.dataLocation >> 8) & 0xff;
                    feBody2[166] = (myFileEntyPosition.dataLocation >> 16) & 0xff;
                    feBody2[167] = (myFileEntyPosition.dataLocation >> 24) & 0xff;
                    const descriptorCRC3 = checkSum(feBody2);
                    fe2[0] = (261 >> 0) & 0xff;
                    fe2[1] = (261 >> 8) & 0xff;
                    fe2[2] = (2 >> 0) & 0xff;
                    fe2[3] = (2 >> 8) & 0xff;
                    fe2[5] = 0x00;
                    fe2[6] = (1 >> 0) & 0xff;
                    fe2[7] = (1 >> 8) & 0xff;
                    fe2[8] = (descriptorCRC3 >> 0) & 0xff;
                    fe2[9] = (descriptorCRC3 >> 8) & 0xff;
                    fe2[10] = ((160 + lAd2) >> 0) & 0xff;
                    fe2[11] = ((160 + lAd2) >> 8) & 0xff;
                    fe2[12] = (myFileEntyPosition.entryLocation >> 0) & 0xff;
                    fe2[13] = (myFileEntyPosition.entryLocation >> 8) & 0xff;
                    fe2[14] = (myFileEntyPosition.entryLocation >> 16) & 0xff;
                    fe2[15] = (myFileEntyPosition.entryLocation >> 24) & 0xff;
                    const checksum3 =
                        (fe2[0] +
                            fe2[1] +
                            fe2[2] +
                            fe2[3] +
                            fe2[5] +
                            fe2[6] +
                            fe2[7] +
                            fe2[8] +
                            fe2[9] +
                            fe2[10] +
                            fe2[11] +
                            fe2[12] +
                            fe2[13] +
                            fe2[14] +
                            fe2[15]) &
                        0xff;
                    fe2[4] = checksum3 & 0xff;
                    for (let vx = 0; vx < feBody2.length; vx++) {
                        fe2[vx + 16] = feBody2[vx];
                    }
                    this.outBlob = new Blob([this.outBlob, fe2]);

                    let currentRealPosition = 2048 * myFileEntyPosition.dataLocation;
                    for (let z = 0; z < everyFidStartPoint.length; z++) {
                        const tagLocationBlock = Math.floor(currentRealPosition / 2048);
                        fids[everyFidStartPoint[z] + 4] -= fids[everyFidStartPoint[z] + 12];
                        fids[everyFidStartPoint[z] + 4] -= fids[everyFidStartPoint[z] + 13];
                        fids[everyFidStartPoint[z] + 4] -= fids[everyFidStartPoint[z] + 14];
                        fids[everyFidStartPoint[z] + 4] -= fids[everyFidStartPoint[z] + 15];
                        fids[everyFidStartPoint[z] + 12] = (tagLocationBlock >> 0) & 0xff;
                        fids[everyFidStartPoint[z] + 13] = (tagLocationBlock >> 8) & 0xff;
                        fids[everyFidStartPoint[z] + 14] = (tagLocationBlock >> 16) & 0xff;
                        fids[everyFidStartPoint[z] + 15] = (tagLocationBlock >> 24) & 0xff;
                        fids[everyFidStartPoint[z] + 4] += fids[everyFidStartPoint[z] + 12];
                        fids[everyFidStartPoint[z] + 4] += fids[everyFidStartPoint[z] + 13];
                        fids[everyFidStartPoint[z] + 4] += fids[everyFidStartPoint[z] + 14];
                        fids[everyFidStartPoint[z] + 4] += fids[everyFidStartPoint[z] + 15];
                        if (z < everyFidStartPoint.length - 1) {
                            currentRealPosition += everyFidStartPoint[z + 1] - everyFidStartPoint[z];
                        }
                    }
                    this.outBlob = new Blob([this.outBlob, fids]);
                    const paddingLen = fidlength % 2048 === 0 ? 0 : 2048 - (fidlength % 2048);
                    const paddingArray = new Uint8Array(paddingLen);
                    this.outBlob = new Blob([this.outBlob, paddingArray]);
                }
            }
        }

        for (const udfImageFile2 of myUDFLayoutInformation.linearUDFImageBuilderFileOrdering) {
            const myFileEntyPos = udfImageFile2.fileEntryPosition;
            if (udfImageFile2.fileType === 'File' && myFileEntyPos.dataBlock !== -1) {
                this.outBlob = new Blob([this.outBlob, udfImageFile2.sourceFile]);
                const paddingLen2 =
                    udfImageFile2.getFileLength() % 2048 === 0 ? 0 : 2048 - (udfImageFile2.getFileLength() % 2048);
                const paddingArr = new Uint8Array(paddingLen2);
                this.outBlob = new Blob([this.outBlob, paddingArr]);
            }
        }

        this.writePVD(myUDFLayoutInformation, myUDFLayoutInformation.pvd2Block, recordingTimeMillis);

        this.writePD(myUDFLayoutInformation, myUDFLayoutInformation.pd2Block);

        this.writeLVD(myUDFLayoutInformation, myUDFLayoutInformation.lvd2Block);

        this.writeUSD(myUDFLayoutInformation, myUDFLayoutInformation.usd2Block);

        this.writeIUVD(myUDFLayoutInformation, myUDFLayoutInformation.iuvd2Block);

        this.writeTD(myUDFLayoutInformation, myUDFLayoutInformation.td2Block);

        const emptyArea5 = new Uint8Array(
            (myUDFLayoutInformation.rvdsEndingBlock - myUDFLayoutInformation.rvdsStartingBlock - 5) * 2048
        );
        this.outBlob = new Blob([this.outBlob, emptyArea5]);

        this.writeAVDP(
            myUDFLayoutInformation,
            myUDFLayoutInformation.avdp2Block,
            myUDFLayoutInformation.mvdsStartingBlock,
            myUDFLayoutInformation.rvdsStartingBlock
        );
        return this.outBlob;
    }
    writeVRS(myUDFLayoutInformation) {
        const vrs1 = new Uint8Array(1 * 2048);
        vrs1[0] = 0x00;
        const vrs1Id = stringToBytes('BEA01');
        for (let i = 0; i < 5; i++) {
            vrs1[i + 1] = vrs1Id[i] & 0xff;
        }
        vrs1[6] = 0x01;
        const vrs2 = new Uint8Array(1 * 2048);
        vrs2[0] = 0x00;
        const vrs2Id = stringToBytes('NSR02');
        for (let b = 0; b < 5; b++) {
            vrs2[b + 1] = vrs2Id[b] & 0xff;
        }
        vrs2[6] = 0x01;
        const vrs3 = new Uint8Array(1 * 2048);
        vrs3[0] = 0x00;
        const vrs3Id = stringToBytes('TEA01');
        for (let v = 0; v < 5; v++) {
            vrs3[v + 1] = vrs3Id[v] & 0xff;
        }
        vrs3[6] = 0x01;
        this.outBlob = new Blob([this.outBlob, vrs1, vrs2, vrs3]);
    }
    writeAVDP(myUDFLayoutInformation, tagLocation, mvdsBlock, rvdsBlock) {
        const avdp = new Uint8Array(1 * 2048);
        const avdpBody = new Uint8Array(496);
        const mvdsLen = 16 * 2048;
        avdpBody[0] = (mvdsLen >> 0) & 0xff;
        avdpBody[1] = (mvdsLen >> 8) & 0xff;
        avdpBody[2] = (mvdsLen >> 16) & 0xff;
        avdpBody[3] = (mvdsLen >> 24) & 0xff;
        const mvdsLoc = mvdsBlock;
        avdpBody[4] = (mvdsLoc >> 0) & 0xff;
        avdpBody[5] = (mvdsLoc >> 8) & 0xff;
        avdpBody[6] = (mvdsLoc >> 16) & 0xff;
        avdpBody[7] = (mvdsLoc >> 24) & 0xff;
        const rvdsLen = 16 * 2048;
        avdpBody[8] = (rvdsLen >> 0) & 0xff;
        avdpBody[9] = (rvdsLen >> 8) & 0xff;
        avdpBody[10] = (rvdsLen >> 16) & 0xff;
        avdpBody[11] = (rvdsLen >> 24) & 0xff;
        const rvdsLoc = rvdsBlock;
        avdpBody[12] = (rvdsLoc >> 0) & 0xff;
        avdpBody[13] = (rvdsLoc >> 8) & 0xff;
        avdpBody[14] = (rvdsLoc >> 16) & 0xff;
        avdpBody[15] = (rvdsLoc >> 24) & 0xff;
        const descriptorCRC = checkSum(avdpBody);
        avdp[0] = (2 >> 0) & 0xff;
        avdp[1] = (2 >> 8) & 0xff;
        avdp[2] = (2 >> 0) & 0xff;
        avdp[3] = (2 >> 8) & 0xff;
        avdp[5] = 0x00;
        avdp[6] = (1 >> 0) & 0xff;
        avdp[7] = (1 >> 8) & 0xff;
        avdp[8] = (descriptorCRC >> 0) & 0xff;
        avdp[9] = (descriptorCRC >> 8) & 0xff;
        avdp[10] = (496 >> 0) & 0xff;
        avdp[11] = (496 >> 8) & 0xff;
        avdp[12] = (tagLocation >> 0) & 0xff;
        avdp[13] = (tagLocation >> 8) & 0xff;
        avdp[14] = (tagLocation >> 16) & 0xff;
        avdp[15] = (tagLocation >> 24) & 0xff;
        const checksum =
            (avdp[0] +
                avdp[1] +
                avdp[2] +
                avdp[3] +
                avdp[5] +
                avdp[6] +
                avdp[7] +
                avdp[8] +
                avdp[9] +
                avdp[10] +
                avdp[11] +
                avdp[12] +
                avdp[13] +
                avdp[14] +
                avdp[15]) &
            0xff;
        avdp[4] = checksum & 0xff;

        for (let i = 0; i < avdpBody.length; i++) {
            avdp[i + 16] = avdpBody[i];
        }
        this.outBlob = new Blob([this.outBlob, avdp]);
    }
    writePVD(myUDFLayoutInformation, pvdBlock, recordingTimeMillis) {
        const pvd = new Uint8Array(1 * 2048);
        const pvdBody = new Uint8Array(496);
        pvdBody[0] = (1 >> 0) & 0xff;
        pvdBody[1] = (1 >> 8) & 0xff;
        pvdBody[2] = (1 >> 16) & 0xff;
        pvdBody[3] = (1 >> 24) & 0xff;
        pvdBody[4] = (0 >> 0) & 0xff;
        pvdBody[5] = (0 >> 8) & 0xff;
        pvdBody[6] = (0 >> 16) & 0xff;
        pvdBody[7] = (0 >> 24) & 0xff;
        const volumeIdentifier = stringToBytes1(this.imageIdentifier);
        for (let i = 0; i < volumeIdentifier.length; i++) {
            pvdBody[i + 8] = volumeIdentifier[i] & 0xff;
        }
        pvdBody[39] = volumeIdentifier.length & 0xff;
        pvdBody[40] = (1 >> 0) & 0xff;
        pvdBody[41] = (1 >> 8) & 0xff;
        pvdBody[42] = (1 >> 0) & 0xff;
        pvdBody[43] = (1 >> 8) & 0xff;
        pvdBody[44] = (2 >> 0) & 0xff;
        pvdBody[45] = (2 >> 8) & 0xff;
        pvdBody[46] = (3 >> 0) & 0xff;
        pvdBody[47] = (3 >> 8) & 0xff;
        pvdBody[48] = (1 >> 0) & 0xff;
        pvdBody[49] = (1 >> 8) & 0xff;
        pvdBody[50] = (1 >> 16) & 0xff;
        pvdBody[51] = (1 >> 24) & 0xff;
        pvdBody[52] = (1 >> 0) & 0xff;
        pvdBody[53] = (1 >> 8) & 0xff;
        pvdBody[54] = (1 >> 16) & 0xff;
        pvdBody[55] = (1 >> 24) & 0xff;
        const volumeSetIdentifier = stringToBytes1(recordingTimeMillis.toString(16) + ' ' + this.imageIdentifier);
        for (let n = 0; n < volumeSetIdentifier.length; n++) {
            pvdBody[n + 56] = volumeSetIdentifier[n] & 0xff;
        }
        pvdBody[56 + 127] = volumeSetIdentifier.length & 0xff;
        pvdBody[184] = 0x00;
        const descriptorCharacterSet = stringToBytes('OSTA Compressed Unicode');
        for (let l = 0; l < descriptorCharacterSet.length; l++) {
            pvdBody[l + 185] = descriptorCharacterSet[l] & 0xff;
        }
        pvdBody[248] = 0x00;
        const specExplanatoryCharacterSet = stringToBytes('OSTA Compressed Unicode');
        for (let v = 0; v < specExplanatoryCharacterSet.length; v++) {
            pvdBody[v + 249] = specExplanatoryCharacterSet[v] & 0xff;
        }
        pvdBody[328] = 0x00;
        const applicationIdentifier = stringToBytes(this.applicationIdentifier);
        for (let u = 0; u < applicationIdentifier.length; u++) {
            pvdBody[u + 329] = applicationIdentifier[u] & 0xff;
        }
        for (let x = 0; x < 8; x++) {
            pvdBody[x + 329 + 23] = this.applicationIdentifierSuffix[x];
        }
        const mytimestamp = millis2TimeStamp(recordingTimeMillis);
        for (let y = 0; y < 12; y++) {
            pvdBody[y + 360] = mytimestamp[y];
        }
        pvdBody[372] = 0x00;
        const implementationIdentifier = stringToBytes(this.applicationIdentifier);
        for (let p = 0; p < implementationIdentifier.length; p++) {
            pvdBody[p + 373] = implementationIdentifier[p] & 0xff;
        }
        pvdBody[468] = 0x00;
        pvdBody[469] = 0x00;
        pvdBody[470] = 0x00;
        pvdBody[471] = 0x00;
        pvdBody[472] = (1 >> 0) & 0xff;
        pvdBody[473] = (1 >> 8) & 0xff;

        const descriptorCRC = checkSum(pvdBody);
        pvd[0] = (1 >> 0) & 0xff;
        pvd[1] = (1 >> 8) & 0xff;
        pvd[2] = (2 >> 0) & 0xff;
        pvd[3] = (2 >> 8) & 0xff;
        pvd[5] = 0x00;
        pvd[6] = (1 >> 0) & 0xff;
        pvd[7] = (1 >> 8) & 0xff;
        pvd[8] = (descriptorCRC >> 0) & 0xff;
        pvd[9] = (descriptorCRC >> 8) & 0xff;
        pvd[10] = (496 >> 0) & 0xff;
        pvd[11] = (496 >> 8) & 0xff;
        pvd[12] = (pvdBlock >> 0) & 0xff;
        pvd[13] = (pvdBlock >> 8) & 0xff;
        pvd[14] = (pvdBlock >> 16) & 0xff;
        pvd[15] = (pvdBlock >> 24) & 0xff;
        const checksum =
            (pvd[0] +
                pvd[1] +
                pvd[2] +
                pvd[3] +
                pvd[5] +
                pvd[6] +
                pvd[7] +
                pvd[8] +
                pvd[9] +
                pvd[10] +
                pvd[11] +
                pvd[12] +
                pvd[13] +
                pvd[14] +
                pvd[15]) &
            0xff;
        pvd[4] = checksum & 0xff;

        for (let j = 0; j < pvdBody.length; j++) {
            pvd[j + 16] = pvdBody[j];
        }
        this.outBlob = new Blob([this.outBlob, pvd]);
    }
    writePD(myUDFLayoutInformation, pdBlock) {
        const pd = new Uint8Array(1 * 2048);
        const pdBody = new Uint8Array(496);
        pdBody[0] = (2 >> 0) & 0xff;
        pdBody[1] = (2 >> 8) & 0xff;
        pdBody[2] = (2 >> 16) & 0xff;
        pdBody[3] = (2 >> 24) & 0xff;
        pdBody[4] = (1 >> 0) & 0xff;
        pdBody[5] = (1 >> 8) & 0xff;
        pdBody[6] = (0 >> 0) & 0xff;
        pdBody[7] = (0 >> 8) & 0xff;
        pdBody[8] = 0x00;
        const partitionContentIdentifie = stringToBytes('+NSR02');
        for (let i = 0; i < partitionContentIdentifie.length; i++) {
            pdBody[9 + i] = partitionContentIdentifie[i] & 0xff;
        }
        pdBody[168] = (1 >> 0) & 0xff;
        pdBody[169] = (1 >> 8) & 0xff;
        pdBody[170] = (1 >> 16) & 0xff;
        pdBody[171] = (1 >> 24) & 0xff;
        pdBody[172] = (myUDFLayoutInformation.physicalPartitionStartingBlock >> 0) & 0xff;
        pdBody[173] = (myUDFLayoutInformation.physicalPartitionStartingBlock >> 8) & 0xff;
        pdBody[174] = (myUDFLayoutInformation.physicalPartitionStartingBlock >> 16) & 0xff;
        pdBody[175] = (myUDFLayoutInformation.physicalPartitionStartingBlock >> 24) & 0xff;
        const pdPartLength =
            myUDFLayoutInformation.physicalPartitionEndingBlock - myUDFLayoutInformation.physicalPartitionStartingBlock;
        pdBody[176] = (pdPartLength >> 0) & 0xff;
        pdBody[177] = (pdPartLength >> 8) & 0xff;
        pdBody[178] = (pdPartLength >> 16) & 0xff;
        pdBody[179] = (pdPartLength >> 24) & 0xff;
        pdBody[180] = 0x00;
        const implementationIdentifier = stringToBytes(this.applicationIdentifier);
        const implementationIdentifierSuffix = stringToBytes(this.applicationIdentifierSuffix);
        for (let j = 0; j < implementationIdentifier.length; j++) {
            pdBody[181 + j] = implementationIdentifier[j] & 0xff;
        }
        for (let k = 0; k < implementationIdentifierSuffix.length; k++) {
            pdBody[204 + k] = implementationIdentifierSuffix[k] & 0xff;
        }
        const descriptorCRC = checkSum(pdBody);
        pd[0] = (5 >> 0) & 0xff;
        pd[1] = (5 >> 8) & 0xff;
        pd[2] = (2 >> 0) & 0xff;
        pd[3] = (2 >> 8) & 0xff;
        pd[5] = 0x00;
        pd[6] = (1 >> 0) & 0xff;
        pd[7] = (1 >> 8) & 0xff;
        pd[8] = (descriptorCRC >> 0) & 0xff;
        pd[9] = (descriptorCRC >> 8) & 0xff;
        pd[10] = (496 >> 0) & 0xff;
        pd[11] = (496 >> 8) & 0xff;
        pd[12] = (pdBlock >> 0) & 0xff;
        pd[13] = (pdBlock >> 8) & 0xff;
        pd[14] = (pdBlock >> 16) & 0xff;
        pd[15] = (pdBlock >> 24) & 0xff;
        const checksum =
            (pd[0] +
                pd[1] +
                pd[2] +
                pd[3] +
                pd[5] +
                pd[6] +
                pd[7] +
                pd[8] +
                pd[9] +
                pd[10] +
                pd[11] +
                pd[12] +
                pd[13] +
                pd[14] +
                pd[15]) &
            0xff;
        pd[4] = checksum & 0xff;
        for (let o = 0; o < pdBody.length; o++) {
            pd[o + 16] = pdBody[o];
        }
        this.outBlob = new Blob([this.outBlob, pd]);
    }
    writeLVD(myUDFLayoutInformation, lvdBlock) {
        const lvd = new Uint8Array(1 * 2048);
        const lvdBody = new Uint8Array(430);
        lvdBody[0] = (3 >> 0) & 0xff;
        lvdBody[1] = (3 >> 8) & 0xff;
        lvdBody[2] = (3 >> 16) & 0xff;
        lvdBody[3] = (3 >> 24) & 0xff;
        lvdBody[4] = 0x00;
        const descriptorCharacterSet = stringToBytes('OSTA Compressed Unicode');
        for (let i = 0; i < descriptorCharacterSet.length; i++) {
            lvdBody[i + 5] = descriptorCharacterSet[i] & 0xff;
        }
        const logicalVolumeIdentifier = stringToBytes1(this.imageIdentifier);
        for (let b = 0; b < logicalVolumeIdentifier.length; b++) {
            lvdBody[b + 68] = logicalVolumeIdentifier[b];
        }
        lvdBody[195] = logicalVolumeIdentifier.length & 0xff;
        lvdBody[196] = (2048 >> 0) & 0xff;
        lvdBody[197] = (2048 >> 8) & 0xff;
        lvdBody[198] = (2048 >> 16) & 0xff;
        lvdBody[199] = (2048 >> 24) & 0xff;
        lvdBody[200] = 0x00;
        const domainIdentifier = stringToBytes('*OSTA UDF Compliant');
        for (let v = 0; v < domainIdentifier.length; v++) {
            lvdBody[201 + v] = domainIdentifier[v];
        }
        for (let g = 0; g < 8; g++) {
            lvdBody[224 + g] = this.udfVersionIdentifierSuffix[g];
        }
        lvdBody[232] = (2048 >> 0) & 0xff;
        lvdBody[233] = (2048 >> 8) & 0xff;
        lvdBody[234] = (2048 >> 16) & 0xff;
        lvdBody[235] = (2048 >> 24) & 0xff;

        lvdBody[236] = (myUDFLayoutInformation.fsdLocation >> 0) & 0xff;
        lvdBody[237] = (myUDFLayoutInformation.fsdLocation >> 8) & 0xff;
        lvdBody[238] = (myUDFLayoutInformation.fsdLocation >> 16) & 0xff;
        lvdBody[239] = (myUDFLayoutInformation.fsdLocation >> 24) & 0xff;

        lvdBody[240] = (myUDFLayoutInformation.partitionToStoreMetadataOn >> 16) & 0xff;
        lvdBody[241] = (myUDFLayoutInformation.partitionToStoreMetadataOn >> 24) & 0xff;
        lvdBody[248] = (6 >> 0) & 0xff;
        lvdBody[249] = (6 >> 8) & 0xff;
        lvdBody[250] = (6 >> 16) & 0xff;
        lvdBody[251] = (6 >> 24) & 0xff;
        lvdBody[252] = (1 >> 0) & 0xff;
        lvdBody[253] = (1 >> 8) & 0xff;
        lvdBody[254] = (1 >> 16) & 0xff;
        lvdBody[255] = (1 >> 24) & 0xff;
        lvdBody[256] = 0x00;
        const implementationIdentifier = stringToBytes(this.applicationIdentifier);
        for (let k = 0; k < implementationIdentifier.length; k++) {
            lvdBody[k + 257] = implementationIdentifier[k] & 0xff;
        }
        for (let x = 0; x < 8; x++) {
            lvdBody[x + 280] = this.applicationIdentifierSuffix[x];
        }
        const lvidLength = (myUDFLayoutInformation.lvidsEndingBlock - myUDFLayoutInformation.lvidsStartingBlock) * 2048;
        lvdBody[416] = (lvidLength >> 0) & 0xff;
        lvdBody[417] = (lvidLength >> 8) & 0xff;
        lvdBody[418] = (lvidLength >> 16) & 0xff;
        lvdBody[419] = (lvidLength >> 24) & 0xff;

        lvdBody[420] = (myUDFLayoutInformation.lvidsStartingBlock >> 0) & 0xff;
        lvdBody[421] = (myUDFLayoutInformation.lvidsStartingBlock >> 8) & 0xff;
        lvdBody[422] = (myUDFLayoutInformation.lvidsStartingBlock >> 16) & 0xff;
        lvdBody[423] = (myUDFLayoutInformation.lvidsStartingBlock >> 24) & 0xff;
        lvdBody[424] = 1 & 0xff;
        lvdBody[425] = 6 & 0xff;
        lvdBody[426] = (1 >> 0) & 0xff;
        lvdBody[427] = (1 >> 8) & 0xff;
        lvdBody[428] = 0x00;
        lvdBody[429] = 0x00;
        const descriptorCRC = checkSum(lvdBody);
        lvd[0] = (6 >> 0) & 0xff;
        lvd[1] = (6 >> 8) & 0xff;
        lvd[2] = (2 >> 0) & 0xff;
        lvd[3] = (2 >> 8) & 0xff;
        lvd[5] = 0x00;
        lvd[6] = (1 >> 0) & 0xff;
        lvd[7] = (1 >> 8) & 0xff;
        lvd[8] = (descriptorCRC >> 0) & 0xff;
        lvd[9] = (descriptorCRC >> 8) & 0xff;
        lvd[10] = (430 >> 0) & 0xff;
        lvd[11] = (430 >> 8) & 0xff;
        lvd[12] = (lvdBlock >> 0) & 0xff;
        lvd[13] = (lvdBlock >> 8) & 0xff;
        lvd[14] = (lvdBlock >> 16) & 0xff;
        lvd[15] = (lvdBlock >> 24) & 0xff;
        const checksum =
            (lvd[0] +
                lvd[1] +
                lvd[2] +
                lvd[3] +
                lvd[5] +
                lvd[6] +
                lvd[7] +
                lvd[8] +
                lvd[9] +
                lvd[10] +
                lvd[11] +
                lvd[12] +
                lvd[13] +
                lvd[14] +
                lvd[15]) &
            0xff;
        lvd[4] = checksum & 0xff;
        for (let f = 0; f < lvdBody.length; f++) {
            lvd[f + 16] = lvdBody[f];
        }
        this.outBlob = new Blob([this.outBlob, lvd]);
    }
    writeUSD(myUDFLayoutInformation, usdBlock) {
        const usd = new Uint8Array(1 * 2048);
        const usdBody = new Uint8Array(16);
        usdBody[0] = (4 >> 0) & 0xff;
        usdBody[1] = (4 >> 8) & 0xff;
        usdBody[2] = (4 >> 16) & 0xff;
        usdBody[3] = (4 >> 24) & 0xff;
        usdBody[4] = (1 >> 0) & 0xff;
        usdBody[5] = (1 >> 8) & 0xff;
        usdBody[6] = (1 >> 16) & 0xff;
        usdBody[7] = (1 >> 24) & 0xff;
        const usdLength = (256 - 19) * 2048;
        usdBody[8] = (usdLength >> 0) & 0xff;
        usdBody[9] = (usdLength >> 8) & 0xff;
        usdBody[10] = (usdLength >> 16) & 0xff;
        usdBody[11] = (usdLength >> 24) & 0xff;

        usdBody[12] = (19 >> 0) & 0xff;
        usdBody[13] = (19 >> 8) & 0xff;
        usdBody[14] = (19 >> 16) & 0xff;
        usdBody[15] = (19 >> 24) & 0xff;

        const descriptorCRC = checkSum(usdBody);
        usd[0] = (7 >> 0) & 0xff;
        usd[1] = (7 >> 8) & 0xff;
        usd[2] = (2 >> 0) & 0xff;
        usd[3] = (2 >> 8) & 0xff;
        usd[5] = 0x00;
        usd[6] = (1 >> 0) & 0xff;
        usd[7] = (1 >> 8) & 0xff;
        usd[8] = (descriptorCRC >> 0) & 0xff;
        usd[9] = (descriptorCRC >> 8) & 0xff;
        usd[10] = (16 >> 0) & 0xff;
        usd[11] = (16 >> 8) & 0xff;
        usd[12] = (usdBlock >> 0) & 0xff;
        usd[13] = (usdBlock >> 8) & 0xff;
        usd[14] = (usdBlock >> 16) & 0xff;
        usd[15] = (usdBlock >> 24) & 0xff;
        const checksum =
            (usd[0] +
                usd[1] +
                usd[2] +
                usd[3] +
                usd[5] +
                usd[6] +
                usd[7] +
                usd[8] +
                usd[9] +
                usd[10] +
                usd[11] +
                usd[12] +
                usd[13] +
                usd[14] +
                usd[15]) &
            0xff;
        usd[4] = checksum & 0xff;
        for (let i = 0; i < usdBody.length; i++) {
            usd[i + 16] = usdBody[i];
        }
        this.outBlob = new Blob([this.outBlob, usd]);
    }
    writeIUVD(myUDFLayoutInformation, iuvdBlock) {
        const iuvd = new Uint8Array(1 * 2048);
        const iuvdBody = new Uint8Array(496);
        iuvdBody[0] = (5 >> 0) & 0xff;
        iuvdBody[1] = (5 >> 8) & 0xff;
        iuvdBody[2] = (5 >> 16) & 0xff;
        iuvdBody[3] = (5 >> 24) & 0xff;
        iuvdBody[4] = 0x00;
        const implementationIdentifier = stringToBytes('*UDF LV Info');
        for (let i = 0; i < implementationIdentifier.length; i++) {
            iuvdBody[i + 5] = implementationIdentifier[i] & 0xff;
        }
        for (let s = 0; s < 8; s++) {
            iuvdBody[s + 28] = this.udfVersionIdentifierSuffix[s];
        }
        iuvdBody[36] = 0x00;
        const descriptorCharacterSet = stringToBytes('OSTA Compressed Unicode');
        for (let c = 0; c < descriptorCharacterSet.length; c++) {
            iuvdBody[c + 37] = descriptorCharacterSet[c] & 0xff;
        }
        const logicalVolumeIdentifier = stringToBytes1(this.imageIdentifier);
        for (let q = 0; q < logicalVolumeIdentifier.length; q++) {
            iuvdBody[q + 100] = logicalVolumeIdentifier[q];
        }
        iuvdBody[227] = logicalVolumeIdentifier.length & 0xff;
        iuvdBody[336] = 0x00;
        const implementationIdentifier2 = stringToBytes(this.applicationIdentifier);
        const implementationIdentifierSuffix = stringToBytes(this.applicationIdentifierSuffix);
        for (let t = 0; t < implementationIdentifier2.length; t++) {
            iuvdBody[337 + t] = implementationIdentifier2[t] & 0xff;
        }
        for (let v = 0; v < implementationIdentifierSuffix.length; v++) {
            iuvdBody[360 + v] = implementationIdentifierSuffix[v] & 0xff;
        }
        const descriptorCRC = checkSum(iuvdBody);
        iuvd[0] = (4 >> 0) & 0xff;
        iuvd[1] = (4 >> 8) & 0xff;
        iuvd[2] = (2 >> 0) & 0xff;
        iuvd[3] = (2 >> 8) & 0xff;
        iuvd[5] = 0x00;
        iuvd[6] = (1 >> 0) & 0xff;
        iuvd[7] = (1 >> 8) & 0xff;
        iuvd[8] = (descriptorCRC >> 0) & 0xff;
        iuvd[9] = (descriptorCRC >> 8) & 0xff;
        iuvd[10] = (496 >> 0) & 0xff;
        iuvd[11] = (496 >> 8) & 0xff;
        iuvd[12] = (iuvdBlock >> 0) & 0xff;
        iuvd[13] = (iuvdBlock >> 8) & 0xff;
        iuvd[14] = (iuvdBlock >> 16) & 0xff;
        iuvd[15] = (iuvdBlock >> 24) & 0xff;
        const checksum =
            (iuvd[0] +
                iuvd[1] +
                iuvd[2] +
                iuvd[3] +
                iuvd[5] +
                iuvd[6] +
                iuvd[7] +
                iuvd[8] +
                iuvd[9] +
                iuvd[10] +
                iuvd[11] +
                iuvd[12] +
                iuvd[13] +
                iuvd[14] +
                iuvd[15]) &
            0xff;
        iuvd[4] = checksum & 0xff;
        for (let u = 0; u < iuvdBody.length; u++) {
            iuvd[u + 16] = iuvdBody[u];
        }
        this.outBlob = new Blob([this.outBlob, iuvd]);
    }
    writeTD(myUDFLayoutInformation, tdBlock) {
        const td = new Uint8Array(1 * 2048);
        const tdBody = new Uint8Array(496);
        const descriptorCRC = checkSum(tdBody);
        td[0] = (8 >> 0) & 0xff;
        td[1] = (8 >> 8) & 0xff;
        td[2] = (2 >> 0) & 0xff;
        td[3] = (2 >> 8) & 0xff;
        td[5] = 0x00;
        td[6] = (1 >> 0) & 0xff;
        td[7] = (1 >> 8) & 0xff;
        td[8] = (descriptorCRC >> 0) & 0xff;
        td[9] = (descriptorCRC >> 8) & 0xff;
        td[10] = (496 >> 0) & 0xff;
        td[11] = (496 >> 8) & 0xff;
        td[12] = (tdBlock >> 0) & 0xff;
        td[13] = (tdBlock >> 8) & 0xff;
        td[14] = (tdBlock >> 16) & 0xff;
        td[15] = (tdBlock >> 24) & 0xff;
        const checksum =
            (td[0] +
                td[1] +
                td[2] +
                td[3] +
                td[5] +
                td[6] +
                td[7] +
                td[8] +
                td[9] +
                td[10] +
                td[11] +
                td[12] +
                td[13] +
                td[14] +
                td[15]) &
            0xff;
        td[4] = checksum & 0xff;
        for (let i = 0; i < tdBody.length; i++) {
            td[i + 16] = tdBody[i];
        }
        this.outBlob = new Blob([this.outBlob, td]);
    }
    writeLVID(myUDFLayoutInformation, recordingTimeMillis) {
        const lvid = new Uint8Array(1 * 2048);
        const lvidBody = new Uint8Array(118);
        const mytimestamp = millis2TimeStamp(recordingTimeMillis);
        for (let i = 0; i < 12; i++) {
            lvidBody[i] = mytimestamp[i];
        }
        lvidBody[12] = (1 >> 0) & 0xff;
        lvidBody[13] = (1 >> 8) & 0xff;
        lvidBody[14] = (1 >> 16) & 0xff;
        lvidBody[15] = (1 >> 24) & 0xff;
        const nextUniqueId = rightShift64(myUDFLayoutInformation.nextUniqueId);
        for (let h = 0; h < 8; h++) {
            lvidBody[h + 24] = nextUniqueId[h];
        }
        lvidBody[56] = (1 >> 0) & 0xff;
        lvidBody[57] = (1 >> 8) & 0xff;
        lvidBody[58] = (1 >> 16) & 0xff;
        lvidBody[59] = (1 >> 24) & 0xff;
        lvidBody[60] = (46 >> 0) & 0xff;
        lvidBody[61] = (46 >> 8) & 0xff;
        lvidBody[62] = (46 >> 16) & 0xff;
        lvidBody[63] = (46 >> 24) & 0xff;
        const sizeTable =
            myUDFLayoutInformation.physicalPartitionEndingBlock - myUDFLayoutInformation.physicalPartitionStartingBlock;
        lvidBody[68] = (sizeTable >> 0) & 0xff;
        lvidBody[69] = (sizeTable >> 8) & 0xff;
        lvidBody[70] = (sizeTable >> 16) & 0xff;
        lvidBody[71] = (sizeTable >> 24) & 0xff;
        lvidBody[72] = 0x00;
        const applicationIdentifier = stringToBytes(this.applicationIdentifier);
        for (let e = 0; e < applicationIdentifier.length; e++) {
            lvidBody[e + 73] = applicationIdentifier[e] & 0xff;
        }
        for (let j = 0; j < 8; j++) {
            lvidBody[j + 96] = this.applicationIdentifierSuffix[j];
        }
        lvidBody[104] = (myUDFLayoutInformation.fileCount >> 0) & 0xff;
        lvidBody[105] = (myUDFLayoutInformation.fileCount >> 8) & 0xff;
        lvidBody[106] = (myUDFLayoutInformation.fileCount >> 16) & 0xff;
        lvidBody[107] = (myUDFLayoutInformation.fileCount >> 24) & 0xff;
        lvidBody[108] = (myUDFLayoutInformation.directoryCount >> 0) & 0xff;
        lvidBody[109] = (myUDFLayoutInformation.directoryCount >> 8) & 0xff;
        lvidBody[110] = (myUDFLayoutInformation.directoryCount >> 16) & 0xff;
        lvidBody[111] = (myUDFLayoutInformation.directoryCount >> 24) & 0xff;
        lvidBody[112] = 0x02;
        lvidBody[113] = 0x01;
        lvidBody[114] = 0x02;
        lvidBody[115] = 0x01;
        lvidBody[116] = 0x02;
        lvidBody[117] = 0x01;
        const descriptorCRC = checkSum(lvidBody);
        lvid[0] = (9 >> 0) & 0xff;
        lvid[1] = (9 >> 8) & 0xff;
        lvid[2] = (2 >> 0) & 0xff;
        lvid[3] = (2 >> 8) & 0xff;
        lvid[5] = 0x00;
        lvid[6] = (1 >> 0) & 0xff;
        lvid[7] = (1 >> 8) & 0xff;
        lvid[8] = (descriptorCRC >> 0) & 0xff;
        lvid[9] = (descriptorCRC >> 8) & 0xff;
        lvid[10] = (118 >> 0) & 0xff;
        lvid[11] = (118 >> 8) & 0xff;
        lvid[12] = (myUDFLayoutInformation.lvidsStartingBlock >> 0) & 0xff;
        lvid[13] = (myUDFLayoutInformation.lvidsStartingBlock >> 8) & 0xff;
        lvid[14] = (myUDFLayoutInformation.lvidsStartingBlock >> 16) & 0xff;
        lvid[15] = (myUDFLayoutInformation.lvidsStartingBlock >> 24) & 0xff;
        const checksum =
            (lvid[0] +
                lvid[1] +
                lvid[2] +
                lvid[3] +
                lvid[5] +
                lvid[6] +
                lvid[7] +
                lvid[8] +
                lvid[9] +
                lvid[10] +
                lvid[11] +
                lvid[12] +
                lvid[13] +
                lvid[14] +
                lvid[15]) &
            0xff;
        lvid[4] = checksum & 0xff;
        for (let p = 0; p < lvidBody.length; p++) {
            lvid[p + 16] = lvidBody[p];
        }
        this.outBlob = new Blob([this.outBlob, lvid]);
    }
    writeFSD(myUDFLayoutInformation, recordingTimeMillis) {
        const fsd = new Uint8Array(1 * 2048);
        const fsdBody = new Uint8Array(496);
        const mytimestamp = millis2TimeStamp(recordingTimeMillis);
        for (let i = 0; i < 12; i++) {
            fsdBody[i] = mytimestamp[i];
        }
        fsdBody[12] = (3 >> 0) & 0xff;
        fsdBody[13] = (3 >> 8) & 0xff;
        fsdBody[14] = (3 >> 0) & 0xff;
        fsdBody[15] = (3 >> 8) & 0xff;
        fsdBody[16] = (1 >> 0) & 0xff;
        fsdBody[17] = (1 >> 8) & 0xff;
        fsdBody[18] = (1 >> 16) & 0xff;
        fsdBody[19] = (1 >> 24) & 0xff;
        fsdBody[20] = (1 >> 0) & 0xff;
        fsdBody[21] = (1 >> 8) & 0xff;
        fsdBody[22] = (1 >> 16) & 0xff;
        fsdBody[23] = (1 >> 24) & 0xff;
        fsdBody[24] = (0 >> 0) & 0xff;
        fsdBody[25] = (0 >> 8) & 0xff;
        fsdBody[26] = (0 >> 16) & 0xff;
        fsdBody[27] = (0 >> 24) & 0xff;
        fsdBody[28] = (0 >> 0) & 0xff;
        fsdBody[29] = (0 >> 8) & 0xff;
        fsdBody[30] = (0 >> 16) & 0xff;
        fsdBody[31] = (0 >> 24) & 0xff;
        fsdBody[32] = 0x00;
        const descriptorCharacterSet = stringToBytes('OSTA Compressed Unicode');
        for (let k = 0; k < descriptorCharacterSet.length; k++) {
            fsdBody[k + 33] = descriptorCharacterSet[k] & 0xff;
        }
        const logicalVolumeIdentifier = stringToBytes1(this.imageIdentifier);
        for (let l = 0; l < logicalVolumeIdentifier.length; l++) {
            fsdBody[l + 96] = logicalVolumeIdentifier[l];
        }
        fsdBody[223] = logicalVolumeIdentifier.length & 0xff;
        fsdBody[224] = 0x00;
        for (let o = 0; o < descriptorCharacterSet.length; o++) {
            fsdBody[o + 225] = descriptorCharacterSet[o] & 0xff;
        }
        const fileSetIdentifier = stringToBytes1(this.imageIdentifier);
        for (let n = 0; n < fileSetIdentifier.length; n++) {
            fsdBody[n + 288] = fileSetIdentifier[n];
        }
        fsdBody[319] = fileSetIdentifier.length & 0xff;
        fsdBody[384] = (2048 >> 0) & 0xff;
        fsdBody[385] = (2048 >> 8) & 0xff;
        fsdBody[386] = (2048 >> 16) & 0xff;
        fsdBody[387] = (2048 >> 24) & 0xff;

        fsdBody[388] = (myUDFLayoutInformation.rootFELocation >> 0) & 0xff;
        fsdBody[389] = (myUDFLayoutInformation.rootFELocation >> 8) & 0xff;
        fsdBody[390] = (myUDFLayoutInformation.rootFELocation >> 16) & 0xff;
        fsdBody[391] = (myUDFLayoutInformation.rootFELocation >> 24) & 0xff;

        fsdBody[392] = (myUDFLayoutInformation.partitionToStoreMetadataOn >> 0) & 0xff;
        fsdBody[393] = (myUDFLayoutInformation.partitionToStoreMetadataOn >> 8) & 0xff;
        fsdBody[400] = 0x00;
        const domainIdentifier = stringToBytes('*OSTA UDF Compliant');
        for (let y = 0; y < domainIdentifier.length; y++) {
            fsdBody[401 + y] = domainIdentifier[y];
        }
        for (let p = 0; p < 8; p++) {
            fsdBody[424 + p] = this.udfVersionIdentifierSuffix[p];
        }
        const descriptorCRC = checkSum(fsdBody);
        fsd[0] = (256 >> 0) & 0xff;
        fsd[1] = (256 >> 8) & 0xff;
        fsd[2] = (2 >> 0) & 0xff;
        fsd[3] = (2 >> 8) & 0xff;
        fsd[5] = 0x00;
        fsd[6] = (1 >> 0) & 0xff;
        fsd[7] = (1 >> 8) & 0xff;
        fsd[8] = (descriptorCRC >> 0) & 0xff;
        fsd[9] = (descriptorCRC >> 8) & 0xff;
        fsd[10] = (496 >> 0) & 0xff;
        fsd[11] = (496 >> 8) & 0xff;
        fsd[12] = (myUDFLayoutInformation.fsdLocation >> 0) & 0xff;
        fsd[13] = (myUDFLayoutInformation.fsdLocation >> 8) & 0xff;
        fsd[14] = (myUDFLayoutInformation.fsdLocation >> 16) & 0xff;
        fsd[15] = (myUDFLayoutInformation.fsdLocation >> 24) & 0xff;
        const checksum =
            (fsd[0] +
                fsd[1] +
                fsd[2] +
                fsd[3] +
                fsd[5] +
                fsd[6] +
                fsd[7] +
                fsd[8] +
                fsd[9] +
                fsd[10] +
                fsd[11] +
                fsd[12] +
                fsd[13] +
                fsd[14] +
                fsd[15]) &
            0xff;
        fsd[4] = checksum & 0xff;
        for (let g = 0; g < fsdBody.length; g++) {
            fsd[g + 16] = fsdBody[g];
        }
        this.outBlob = new Blob([this.outBlob, fsd]);
    }
}

export function stringToBytes(str) {
    if (typeof(str) !== 'string') {
        return str;
    }
    const length = str.length;
    const BYTE_LENGTH = 8;
    const charCodeArr = [];
    for (let i = length - 1; i >= 0; i--) {
        const charCode = str.charCodeAt(i);
        charCodeArr.unshift(charCode & 0xff);
        let rightShiftTimes = 1;
        while ((charCode >>> (BYTE_LENGTH * rightShiftTimes)) > 0) {
            charCodeArr.unshift(charCode >>> rightShiftTimes * BYTE_LENGTH & 0xff);
            rightShiftTimes++;
        }
    }
    return charCodeArr;
}

export function millis2TimeStamp(mills) {
    const mydate = new Date(mills);
    const mytimestamp = new Uint8Array(12);
    let twelveBitSignedValue = mydate.getTimezoneOffset();
    if (twelveBitSignedValue < 0) {
        twelveBitSignedValue *= -1;
    }
    const typeAndTimezone = 0x1000 | twelveBitSignedValue;
    mytimestamp[0] = (typeAndTimezone >> 0) & 0xff;
    mytimestamp[1] = (typeAndTimezone >> 8) & 0xff;

    mytimestamp[2] = (mydate.getFullYear() >> 0) & 0xff;
    mytimestamp[3] = (mydate.getFullYear() >> 8) & 0xff;
    mytimestamp[4] = (mydate.getMonth() + 1) & 0xff;
    mytimestamp[5] = mydate.getDate() & 0xff;
    mytimestamp[6] = mydate.getHours() & 0xff;
    mytimestamp[7] = mydate.getMinutes() & 0xff;
    mytimestamp[8] = mydate.getSeconds() & 0xff;
    mytimestamp[9] = (mydate.getMilliseconds() / 100) & 0xff;
    mytimestamp[10] = mydate.getMilliseconds() % 100 & 0xff;
    mytimestamp[11] = 0x00;

    return mytimestamp;
}

export function rightShift64(sData) {
    const dData = new Uint8Array(8);
    const temp = sData.toString(16);
    const myLength = 16 - temp.length;
    let temp1 = '';
    for (let i = 0; i < myLength; i++) {
        temp1 += '0';
    }
    temp1 += temp;
    dData[0] = Number('0x' + temp1.substring(14, 16)) & 0xff;
    dData[1] = Number('0x' + temp1.substring(12, 14)) & 0xff;
    dData[2] = Number('0x' + temp1.substring(10, 12)) & 0xff;
    dData[3] = Number('0x' + temp1.substring(8, 10)) & 0xff;
    dData[4] = Number('0x' + temp1.substring(6, 8)) & 0xff;
    dData[5] = Number('0x' + temp1.substring(4, 6)) & 0xff;
    dData[6] = Number('0x' + temp1.substring(2, 4)) & 0xff;
    dData[7] = Number('0x' + temp1.substring(0, 2)) & 0xff;
    return dData;
}

export function checkSum(s) {
    const crcTable = [
        0x0000,
        0x1021,
        0x2042,
        0x3063,
        0x4084,
        0x50a5,
        0x60c6,
        0x70e7,
        0x8108,
        0x9129,
        0xa14a,
        0xb16b,
        0xc18c,
        0xd1ad,
        0xe1ce,
        0xf1ef,
        0x1231,
        0x0210,
        0x3273,
        0x2252,
        0x52b5,
        0x4294,
        0x72f7,
        0x62d6,
        0x9339,
        0x8318,
        0xb37b,
        0xa35a,
        0xd3bd,
        0xc39c,
        0xf3ff,
        0xe3de,
        0x2462,
        0x3443,
        0x0420,
        0x1401,
        0x64e6,
        0x74c7,
        0x44a4,
        0x5485,
        0xa56a,
        0xb54b,
        0x8528,
        0x9509,
        0xe5ee,
        0xf5cf,
        0xc5ac,
        0xd58d,
        0x3653,
        0x2672,
        0x1611,
        0x0630,
        0x76d7,
        0x66f6,
        0x5695,
        0x46b4,
        0xb75b,
        0xa77a,
        0x9719,
        0x8738,
        0xf7df,
        0xe7fe,
        0xd79d,
        0xc7bc,
        0x48c4,
        0x58e5,
        0x6886,
        0x78a7,
        0x0840,
        0x1861,
        0x2802,
        0x3823,
        0xc9cc,
        0xd9ed,
        0xe98e,
        0xf9af,
        0x8948,
        0x9969,
        0xa90a,
        0xb92b,
        0x5af5,
        0x4ad4,
        0x7ab7,
        0x6a96,
        0x1a71,
        0x0a50,
        0x3a33,
        0x2a12,
        0xdbfd,
        0xcbdc,
        0xfbbf,
        0xeb9e,
        0x9b79,
        0x8b58,
        0xbb3b,
        0xab1a,
        0x6ca6,
        0x7c87,
        0x4ce4,
        0x5cc5,
        0x2c22,
        0x3c03,
        0x0c60,
        0x1c41,
        0xedae,
        0xfd8f,
        0xcdec,
        0xddcd,
        0xad2a,
        0xbd0b,
        0x8d68,
        0x9d49,
        0x7e97,
        0x6eb6,
        0x5ed5,
        0x4ef4,
        0x3e13,
        0x2e32,
        0x1e51,
        0x0e70,
        0xff9f,
        0xefbe,
        0xdfdd,
        0xcffc,
        0xbf1b,
        0xaf3a,
        0x9f59,
        0x8f78,
        0x9188,
        0x81a9,
        0xb1ca,
        0xa1eb,
        0xd10c,
        0xc12d,
        0xf14e,
        0xe16f,
        0x1080,
        0x00a1,
        0x30c2,
        0x20e3,
        0x5004,
        0x4025,
        0x7046,
        0x6067,
        0x83b9,
        0x9398,
        0xa3fb,
        0xb3da,
        0xc33d,
        0xd31c,
        0xe37f,
        0xf35e,
        0x02b1,
        0x1290,
        0x22f3,
        0x32d2,
        0x4235,
        0x5214,
        0x6277,
        0x7256,
        0xb5ea,
        0xa5cb,
        0x95a8,
        0x8589,
        0xf56e,
        0xe54f,
        0xd52c,
        0xc50d,
        0x34e2,
        0x24c3,
        0x14a0,
        0x0481,
        0x7466,
        0x6447,
        0x5424,
        0x4405,
        0xa7db,
        0xb7fa,
        0x8799,
        0x97b8,
        0xe75f,
        0xf77e,
        0xc71d,
        0xd73c,
        0x26d3,
        0x36f2,
        0x0691,
        0x16b0,
        0x6657,
        0x7676,
        0x4615,
        0x5634,
        0xd94c,
        0xc96d,
        0xf90e,
        0xe92f,
        0x99c8,
        0x89e9,
        0xb98a,
        0xa9ab,
        0x5844,
        0x4865,
        0x7806,
        0x6827,
        0x18c0,
        0x08e1,
        0x3882,
        0x28a3,
        0xcb7d,
        0xdb5c,
        0xeb3f,
        0xfb1e,
        0x8bf9,
        0x9bd8,
        0xabbb,
        0xbb9a,
        0x4a75,
        0x5a54,
        0x6a37,
        0x7a16,
        0x0af1,
        0x1ad0,
        0x2ab3,
        0x3a92,
        0xfd2e,
        0xed0f,
        0xdd6c,
        0xcd4d,
        0xbdaa,
        0xad8b,
        0x9de8,
        0x8dc9,
        0x7c26,
        0x6c07,
        0x5c64,
        0x4c45,
        0x3ca2,
        0x2c83,
        0x1ce0,
        0x0cc1,
        0xef1f,
        0xff3e,
        0xcf5d,
        0xdf7c,
        0xaf9b,
        0xbfba,
        0x8fd9,
        0x9ff8,
        0x6e17,
        0x7e36,
        0x4e55,
        0x5e74,
        0x2e93,
        0x3eb2,
        0x0ed1,
        0x1ef0
    ];
    let crc = 0;
    for (const i of s) {
        crc = crcTable[((crc >> 8) ^ i) & 0xff] ^ (crc << 8);
    }
    return crc & 0xffff;
}

export function getNowTime() {
    const nowTime = new Date();
    const time = nowTime.getMinutes() + ':' + nowTime.getSeconds() + ':' + nowTime.getMilliseconds();
    return time;
}
