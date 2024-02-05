export class DCBAttr {
    public up: [string, number[] | null];
    public pfcup: [string, string];
    public up2cos: [string, string];
    public pgid: [string, string];
    public pGPCT: [string, string];
    public pgStrict: [string, string];
    constructor(
        up: number[] | null,
        pfcup: string,
        up2cos: string,
        pgid: string,
        pGPCT: string,
        pgStrict: string
    ) {
        this.up = ['up', up];
        this.pfcup = ['pfcup', pfcup];
        this.up2cos = ['up2cos', up2cos];
        this.pgid = ['pgid', pgid];
        this.pGPCT = ['pgpct', pGPCT];
        this.pgStrict = ['pg strict', pgStrict];
    }
}
