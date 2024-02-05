import { BootType } from './boot-type';
import { BootTarget } from './boot-target';
import { BootTargetExt } from './bios-interface';

export class Bios {
    public bootModeConfigOverIpmiEnabled: boolean;
    public bootSourceOverrideMode: string;
    public bootSourceOverrideTarget: BootTarget;
    public bootSourceOverrideTargetExt: string[];
    public bootSourceOverrideTargetList: BootTarget[];
    public bootSourceOverrideTargetListExt: BootTargetExt[];
    public bootSourceOverrideEnabled: string;
    public bootModeIpmiSWDisplayEnabled?: boolean;
    public bootTypeOrder: BootType[];
    public cpuPLimit: number;
    public cpuTLimit: number;
    public cpuPState: boolean;
    public cpuTState: boolean;
    public cpuPScales: string[];
    public cpuTScales: string[];

    constructor() { }
}
