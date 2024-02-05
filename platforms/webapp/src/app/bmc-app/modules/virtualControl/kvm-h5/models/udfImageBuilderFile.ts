export class UdfImageBuilderFile {
    public identifier;
    public sourceFile = null;
    public children = new Array();
    public fileType;

    public accessTime;
    public attributeTime;
    public creationTime;
    public modificationTIme;

    public fileLinkCount;
    public parent;
    public fileEntryPosition;
    public sourceFileArray;

    getDirectoryCount() {
        let directoryCount = 0;
        if (this.fileType === 'Directory') {
            directoryCount++;
            for (const childUDFImageBuilderFile of this.children) {
                if (childUDFImageBuilderFile.fileType === 'Directory') {
                    directoryCount += childUDFImageBuilderFile.getDirectoryCount();
                }
            }
        }
        return directoryCount;
    }
    getFileCount() {
        let fileCount = 0;
        if (this.fileType === 'Directory') {
            for (const childUDFImageBuilderFile of this.children) {
                if (childUDFImageBuilderFile.fileType === 'Directory') {
                    fileCount += childUDFImageBuilderFile.getFileCount();
                } else if (childUDFImageBuilderFile.fileType === 'File') {
                    fileCount++;
                }
            }
        } else if (this.fileType === 'File') {
            fileCount = 1;
        }
        return fileCount;
    }
    getFileLength() {
        if (this.sourceFile != null) {
            return this.sourceFile.size;
        } else {
            return 0;
        }
    }
    getChildren() {
        return this.children;
    }
}
