import { stringToBytes1 } from '../kvm-h5.util';
import { UniqueIdDisposer } from './uniqueIdDisposer';
import { FileEntryPositions } from './fileEntryPositions';
export class UdfLayoutInformation {
    public myUniqueIdDisposer;

    public blockSize;

    public metadataAllocationUnitSize;
    public metadataAlignmentUnitSize;

    public fileCount;
    public directoryCount;

    public avdp1Block;
    public avdp2Block;

    public mvdsStartingBlock;
    public mvdsEndingBlock;

    public rvdsStartingBlock;
    public rvdsEndingBlock;

    public lvidsStartingBlock;
    public lvidsEndingBlock;

    public physicalPartitionStartingBlock;
    public physicalPartitionEndingBlock;

    public metadataPartitionStartingBlock;
    public metadataPartitionEndingBlock;

    public mainMetadataFileBlock;
    public mainMetadataFileLocation;

    public mirrorMetadataFileBlock;
    public mirrorMetadataFileLocation;

    public metadataEmptyArea;

    public partitionToStoreMetadataOn;

    public fsdBlock;
    public fsdLocation;

    public rootFEBlock;
    public rootFELocation;

    public pvd1Block;
    public pvd2Block;
    public pd1Block;
    public pd2Block;
    public lvd1Block;
    public lvd2Block;
    public usd1Block;
    public usd2Block;
    public iuvd1Block;
    public iuvd2Block;
    public td1Block;
    public td2Block;

    public sizeTable;
    public freespaceTable;

    public nextUniqueId;

    public linearUDFImageBuilderFileOrdering;

    public fileEntryPositions;

    init(rootUDFImageBuilderFile, myUDFRevision, blockSize) {
        this.myUniqueIdDisposer = new UniqueIdDisposer();

        this.fileEntryPositions = new Array();
        this.linearUDFImageBuilderFileOrdering = new Array();

        this.blockSize = blockSize;
        this.metadataAllocationUnitSize = 32;
        this.metadataAlignmentUnitSize = 1;

        this.fileCount = rootUDFImageBuilderFile.getFileCount();
        this.directoryCount = rootUDFImageBuilderFile.getDirectoryCount();

        this.avdp1Block = 256;

        this.mvdsStartingBlock = 257;
        this.mvdsEndingBlock = this.mvdsStartingBlock + 16;
        this.pvd1Block = this.mvdsStartingBlock;
        this.pd1Block = this.mvdsStartingBlock + 1;
        this.lvd1Block = this.mvdsStartingBlock + 2;
        this.usd1Block = this.mvdsStartingBlock + 3;
        this.iuvd1Block = this.mvdsStartingBlock + 4;
        this.td1Block = this.mvdsStartingBlock + 5;

        this.lvidsStartingBlock = this.mvdsEndingBlock;
        this.lvidsEndingBlock = this.lvidsStartingBlock + 4;

        this.physicalPartitionStartingBlock = this.lvidsEndingBlock;

        this.partitionToStoreMetadataOn = 0;

        this.fsdBlock = this.physicalPartitionStartingBlock + 1;
        this.fsdLocation = 1;
        this.rootFEBlock = this.physicalPartitionStartingBlock + 2;
        this.rootFELocation = 2;

        const currentBlock = this.recursiveGetFileEntryLocation(
            rootUDFImageBuilderFile,
            [this.rootFEBlock, 0],
            myUDFRevision
        );

        this.nextUniqueId = this.myUniqueIdDisposer.getNextUniqueId();
        this.physicalPartitionEndingBlock = currentBlock[0] + currentBlock[1];

        for (const myUdfImageFile of this.linearUDFImageBuilderFileOrdering) {
            if (myUdfImageFile.fileType === 'File') {
                if (Number(myUdfImageFile.fileEntryPosition.dataBlock) !== -1) {
                    myUdfImageFile.fileEntryPosition.dataBlock += currentBlock[0];
                    myUdfImageFile.fileEntryPosition.dataLocation +=
                        myUdfImageFile.fileEntryPosition.dataBlock - this.physicalPartitionStartingBlock;
                }
            }
        }
        this.sizeTable = [];
        this.sizeTable[0] = this.physicalPartitionEndingBlock - this.physicalPartitionStartingBlock;
        this.freespaceTable = [];

        this.rvdsStartingBlock = this.physicalPartitionEndingBlock;
        this.rvdsEndingBlock = this.rvdsStartingBlock + 16;

        this.pvd2Block = this.rvdsStartingBlock;
        this.pd2Block = this.rvdsStartingBlock + 1;
        this.lvd2Block = this.rvdsStartingBlock + 2;
        this.usd2Block = this.rvdsStartingBlock + 3;
        this.iuvd2Block = this.rvdsStartingBlock + 4;
        this.td2Block = this.rvdsStartingBlock + 5;

        this.avdp2Block = this.rvdsEndingBlock + 1;
    }

    recursiveGetFileEntryLocation(currentUDFImageBuilderFile, currentBlock, myUDFRevision) {
        const myFileEntryPosition = new FileEntryPositions();
        let block;
        if (currentUDFImageBuilderFile.identifier === '') {
            myFileEntryPosition.uniqueIds = 0;
        } else {
            myFileEntryPosition.uniqueIds = this.myUniqueIdDisposer.getNextUniqueId();
        }
        this.linearUDFImageBuilderFileOrdering.push(currentUDFImageBuilderFile);

        myFileEntryPosition.entryBlock = currentBlock[0];
        currentBlock[0]++;
        myFileEntryPosition.entryLocation = myFileEntryPosition.entryBlock - this.physicalPartitionStartingBlock;

        if (currentUDFImageBuilderFile.fileType === 'File') {
            if (currentUDFImageBuilderFile.getFileLength() > this.blockSize - 176) {
                myFileEntryPosition.dataBlock = currentBlock[1];
                currentBlock[1] += Math.ceil(currentUDFImageBuilderFile.getFileLength() / this.blockSize);
            } else {
                myFileEntryPosition.dataBlock = -1;
            }
        } else {
            let fidLength = 40;

            const childUDFImageBuilderFiles = currentUDFImageBuilderFile.getChildren();
            for (const cudfImageFile of childUDFImageBuilderFiles) {
                const cidentifier = cudfImageFile.identifier;

                const lengthWithoutPadding = 38 + stringToBytes1(cidentifier).length;
                const paddingLength = lengthWithoutPadding % 4 === 0 ? 0 : 4 - (lengthWithoutPadding % 4);
                const cfidLength = lengthWithoutPadding + paddingLength;
                fidLength += cfidLength;
            }
            if (fidLength > this.blockSize - 176) {
                myFileEntryPosition.dataBlock = currentBlock[0];
                myFileEntryPosition.dataLocation = myFileEntryPosition.dataBlock - this.physicalPartitionStartingBlock;
                currentBlock[0] += Math.ceil(fidLength / this.blockSize);
            } else {
                myFileEntryPosition.dataBlock = -1;
            }
            block = currentBlock;
            for (const childUDFImageBuilderFile of childUDFImageBuilderFiles) {
                block = this.recursiveGetFileEntryLocation(
                    childUDFImageBuilderFile,
                    block,
                    myUDFRevision
                );
            }
        }
        currentUDFImageBuilderFile.fileEntryPosition = myFileEntryPosition;
        return block ? block : currentBlock;
    }
}


