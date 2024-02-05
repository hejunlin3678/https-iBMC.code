export class SendAttr {
    private totalBytesTransmitted: [string, string];
    private unicastPacketsTransmitted: [string, string];
    private multicastPacketsTransmitted: [string, string];
    private broadcastPacketsTransmitted: [string, string];
    private singleCollisionTransmitFrames: [string, string];
    private multipleCollisionTransmitFrames: [string, string];
    private lateCollisionFrames: [string, string];
    private excessiveCollisionFrames: [string, string];
    constructor(
        totalBytesTransmitted: string,
        unicastPacketsTransmitted: string,
        multicastPacketsTransmitted: string,
        broadcastPacketsTransmitted: string,
        singleCollisionTransmitFrames: string,
        multipleCollisionTransmitFrames: string,
        lateCollisionFrames: string,
        excessiveCollisionFrames: string
    ) {
       this.totalBytesTransmitted = ['TOTAL_BYTES_TRANSMITTED', totalBytesTransmitted];
       this.unicastPacketsTransmitted = ['UNICAST_PACKETS_TRANSMITTED', unicastPacketsTransmitted];
       this.multicastPacketsTransmitted = ['MULTICAST_PACKETS_TRANSMITTED', multicastPacketsTransmitted];
       this.broadcastPacketsTransmitted = ['BROAD_CAST_PACKETS_TRANSMITTED', broadcastPacketsTransmitted];
       this.singleCollisionTransmitFrames = ['SINGLE_COLLISION_TRANSMIT_FRAMES', singleCollisionTransmitFrames];
       this.multipleCollisionTransmitFrames = ['MULTIPLE_COLLISION_TRANSMIT_FRAMES', multipleCollisionTransmitFrames];
       this.lateCollisionFrames = ['LATE_COLLISION_FRAMES', lateCollisionFrames];
       this.excessiveCollisionFrames = ['EXCESSIVE_COLLISION_FRAMES', excessiveCollisionFrames];
    }
}
