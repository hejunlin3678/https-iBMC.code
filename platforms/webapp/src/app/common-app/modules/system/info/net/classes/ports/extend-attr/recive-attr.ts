export class RecvAttr {
    private totalBytesReceived: [string, string];
    private unicastPacketsReceived: [string, string];
    private multicastPacketsReceived: [string, string];
    private broadcastPacketsReceived: [string, string];
    private fcsReceiveErrors: [string, string];
    private alignmentErrors: [string, string];
    private runtPacketsReceived: [string, string];
    private jabberPacketsReceived: [string, string];
    constructor(
        totalBytesReceived: string,
        unicastPacketsReceived: string,
        multicastPacketsReceived: string,
        broadcastPacketsReceived: string,
        fcsReceiveErrors: string,
        alignmentErrors: string,
        runtPacketsReceived: string,
        jabberPacketsReceived: string
    ) {
        this.totalBytesReceived = ['TOTAL_BYTES_RECEIVED', totalBytesReceived];
        this.unicastPacketsReceived = ['UNICAST_PACKETS_RECEIVED', unicastPacketsReceived];
        this.multicastPacketsReceived = ['MULTICAST_PACKETS_RECEIVED', multicastPacketsReceived];
        this.broadcastPacketsReceived = ['BROAD_CAST_PACKETS_TRANSMITTED', broadcastPacketsReceived];
        this.fcsReceiveErrors = ['FCS_RECEIVE_ERRORS', fcsReceiveErrors];
        this.alignmentErrors = ['ALIGNMENT_ERRORS', alignmentErrors];
        this.runtPacketsReceived = ['RUNT_PACKETS_RECEIVED', runtPacketsReceived];
        this.jabberPacketsReceived = ['JABBER_PACKETS_RECEIVED', jabberPacketsReceived];
    }

}
