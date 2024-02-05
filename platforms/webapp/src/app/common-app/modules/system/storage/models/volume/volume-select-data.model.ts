import { IRadio, IOptions } from 'src/app/common-app/models';

export class VolumeSelectData {

    public name: string;
    public bootDiskSelect: IRadio;
    public sscdFuncSelect: IRadio;
    public driveCacheSelect: IOptions;
    public accessSelect: IOptions;
    public bgiStateSelect: IOptions;
    public ioPolicySelect: IOptions;
    public writePolicySelect: IOptions;
    public readPolicySelect: IOptions;

    constructor(
        name: string,
        bootDiskSelect: IRadio,
        sscdFuncSelect: IRadio,
        driveCacheSelect: IOptions,
        accessSelect: IOptions,
        bgiStateSelect: IOptions,
        ioPolicySelect: IOptions,
        writePolicySelect: IOptions,
        readPolicySelect: IOptions
    ) {
        this.name = name;
        this.bootDiskSelect = bootDiskSelect;
        this.sscdFuncSelect = sscdFuncSelect;
        this.driveCacheSelect = driveCacheSelect;
        this.accessSelect = accessSelect;
        this.bgiStateSelect = bgiStateSelect;
        this.ioPolicySelect = ioPolicySelect;
        this.writePolicySelect = writePolicySelect;
        this.readPolicySelect = readPolicySelect;
    }
}
