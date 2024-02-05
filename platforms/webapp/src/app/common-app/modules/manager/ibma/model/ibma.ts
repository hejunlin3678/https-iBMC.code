import { IbmaInfo } from './ibma-info';

export class Ibma {
    constructor() { }
    public iBMAInstallableShow: boolean;
    public iBMAInfos: IbmaInfo[];
    public iBMAInserted: boolean;
    public iBMAInsertedStr: string;
    public iBMAVersion: string;
    public iBMARunStatus: string;
    public iBMADirverVersion: string;
}
