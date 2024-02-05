import { Injectable } from '@angular/core';
import { HttpService, UserInfoService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { BootType } from '../model/boot-type';
import { BootTarget } from '../model/boot-target';
import { Bios } from '../model/bios';
import { ICUPData, IBootData, PLATFORM } from '../model/bios-interface';

@Injectable({
    providedIn: 'root'
})
export class BiosService {

    constructor(
        private http: HttpService,
        private translate: TranslateService,
        private user: UserInfoService
    ) { }

    private bootMediaLanguages: object = {
        Hdd: this.translate.instant('BIOS_HAED_DRIVE'),
        Cd: this.translate.instant('BIOS_CD_DVD'),
        Floppy: this.translate.instant('BIOS_FDD_REMOVAVLE_DEVICE'),
        Pxe: this.translate.instant('BIOS_PXE'),
        BiosSetup: this.translate.instant('BIOS_SETTING'),
        None: this.translate.instant('BIOS_NO_OVERRIDE'),
        Usb: this.translate.instant('BIOS_USB'),
        Tftp: this.translate.instant('BIOS_TFTP'),
        Https: this.translate.instant('SERVICE_HTTPS'),
        Emmc: this.translate.instant('BIOS_EMMC'),
        NvmeM2: this.translate.instant('BIOS_NVMEM2'),
        Sata: this.translate.instant('BIOS_SATA'),
    };
    private bootSequenceLanguages: object = {
        HardDiskDrive: this.translate.instant('BIOS_HARD_DISK_DRIVE'),
        DVDROMDrive: this.translate.instant('BIOS_DVD_ROM_DRIVE'),
        PXE: this.translate.instant('BIOS_PXE'),
        Others: this.translate.instant('COMMON_OTHER')
    };
    public getBoot() {
        return new Promise((resolve, reject) => {
            this.http.get('/UI/Rest/System/BootOptions').subscribe((res) => {
                resolve(res);
            });
        });
    }

    public getCPU() {
        return new Promise((resolve, reject) => {
            if (this.user.platform !== PLATFORM.ARM) {
                this.http.get('/UI/Rest/System/CPUAdjust').subscribe((res) => {
                    resolve(res);
                }, (error) => {
                    resolve(false);
                });
            } else {
                resolve(false);
            }
        });
    }

    public setBoot(param) {
        return this.http.patch('/UI/Rest/System/BootOptions', param);
    }

    public setCPU(param) {
        return this.http.patch('/UI/Rest/System/CPUAdjust', param);
    }

    setBootModel(model: Bios, bootData: IBootData) {
        const {
            bootModeConfigOverIpmiEnabled,
            bootSourceOverrideMode,
            bootSourceOverrideTarget,
            bootSourceOverrideTargetList,
            bootModeIpmiSWDisplayEnabled,
            bootSourceOverrideEnabled,
            bootTypeOrder,
            bootSourceOverrideTargetListExt,
            bootSourceOverrideTargetExt
        } = bootData;
        model.bootModeConfigOverIpmiEnabled = bootModeConfigOverIpmiEnabled;
        model.bootSourceOverrideMode = bootSourceOverrideMode;
        model.bootSourceOverrideTarget = bootSourceOverrideTarget;
        model.bootSourceOverrideTargetList = bootSourceOverrideTargetList;
        model.bootModeIpmiSWDisplayEnabled = bootModeIpmiSWDisplayEnabled;
        model.bootSourceOverrideEnabled = bootSourceOverrideEnabled;
        model.bootTypeOrder = bootTypeOrder;
        model.bootSourceOverrideTargetListExt = bootSourceOverrideTargetListExt;
        model.bootSourceOverrideTargetExt = bootSourceOverrideTargetExt;
    }

    setCPUModel(model: Bios, cpuData: ICUPData) {
        const {
            CPUPLimit,
            CPUTLimit,
            CPUPState,
            CPUTState,
            CPUPScales,
            CPUTScales
        } = cpuData;
        model.cpuPLimit = CPUPLimit;
        model.cpuTLimit = CPUTLimit;
        model.cpuPState = CPUPState;
        model.cpuTState = CPUTState;
        model.cpuPScales = CPUPScales;
        model.cpuTScales = CPUTScales;
    }
    public getBootSequence(sequence) {
        // 启动顺序取值:'HardDiskDrive','DVDROMDrive','PXE','Others'
        const arr = [];
        if (sequence) {
            for (const value of sequence) {
                const bootType = new BootType();
                bootType.key = this.bootSequenceLanguages[value];
                bootType.value = value;
                arr.push(bootType);
            }
        }
        return arr;
    }

    getBios() {
        return new Promise((resolve, reject) => {
            Promise.all([
                this.getBoot(),
                this.getCPU()
            ]).then((resArr) => {
                resolve(resArr);
            }).catch((error) => {
                reject(error);
            });
        });
    }

    getBootData(data: any) {
        const _self = this;
        const preferredBootMedium = data?.PreferredBootMedium;
        const deviceTypes = [];
        let bootTarget: BootTarget;
        const bootTargetList: BootTarget[] = [];
        if (preferredBootMedium?.AllowableValues) {
            const allowableValues = preferredBootMedium.AllowableValues;
            allowableValues.forEach((item, index) => {
                let bootIndex = index;
                switch (item) {
                    case 'Hdd':
                        bootIndex = 0;
                        break;
                    case 'Cd':
                        bootIndex = 1;
                        break;
                    case 'Floppy':
                        bootIndex = 2;
                        break;
                    case 'Pxe':
                        bootIndex = 3;
                        break;
                    case 'BiosSetup':
                        bootIndex = 4;
                        break;
                    case 'None':
                        bootIndex = 5;
                        break;
                    case 'USB':
                        bootIndex = 6;
                        break;
                    case 'PCIeM2':
                        bootIndex = 7;
                        break;
                    case 'CoreBoardeMMC':
                        bootIndex = 8;
                        break;
                    default:
                        return;
                }
                const obj = {
                    id: item,
                    label: _self.bootMediaLanguages[item],
                    index: bootIndex
                };
                deviceTypes.push(obj);
            });
            const options = deviceTypes.sort((n1, n2) => n1.index - n2.index);
            bootTarget = new BootTarget();
            options.forEach((option) => {
                const bootTargetItem = new BootTarget();
                bootTargetItem.id = option.id;
                bootTargetItem.label = option.label;
                bootTargetItem.index = option.index;
                bootTargetList.push(bootTargetItem);
                if (option.id === preferredBootMedium.Target) {
                    bootTarget = bootTargetItem;
                }
            });
        }
        const bootTargetListExt = [];
        const bootTargetExt = [];
        if (preferredBootMedium?.AllowableValuesExt) {
            const allowableValuesExt = preferredBootMedium.AllowableValuesExt;
            bootTarget = {id: preferredBootMedium?.Target, label: '', index: 0};
            bootTargetExt.length = 0;
            bootTargetExt.push(preferredBootMedium?.Target);
            Object.keys(allowableValuesExt).forEach((iterator) => {
                const item = {};
                const children = [];
                item['id'] = iterator;
                item['label'] = _self.bootMediaLanguages[iterator];
                if (allowableValuesExt[iterator].length > 0) {
                    allowableValuesExt[iterator].map((i: string) => {
                        if (i === preferredBootMedium?.TargetExt) {
                            bootTargetExt.push(`${i}`);
                        }
                        children.push({'id': `${i}`, 'label':  _self.bootMediaLanguages[i]});
                    });
                    item['children'] = children;
                }
                bootTargetListExt.push(item);
            });
        }
        const bootOrder: BootType[] = this.getBootSequence(data.BootOrder);
        const bootModeIpmiSWDisplayEnabled = data.BootModeIpmiSWDisplayEnabled;
        const bootData: IBootData = {
            bootModeConfigOverIpmiEnabled: data.BootModeConfigOverIpmiEnabled === 'On',
            bootSourceOverrideMode: data.BootSourceOverrideMode,
            bootSourceOverrideTarget: bootTarget,
            bootModeIpmiSWDisplayEnabled,
            bootSourceOverrideTargetList: bootTargetList,
            bootSourceOverrideEnabled: preferredBootMedium.EffectivePeriod === 'Continuous' ? 'Continuous' : 'Once',
            bootTypeOrder: bootOrder,
            bootSourceOverrideTargetListExt: bootTargetListExt,
            bootSourceOverrideTargetExt: bootTargetExt
        };
        return bootData;
    }
    getCPUData(data: any) {
        const cpuPScale = [];
        for (let i = 0; i <= data.PStateLimit; i++) {
            cpuPScale.push(`P${i}`);
        }
        const cpuTScale = [];
        for (let i = 0; i <= data.TStatelimit; i++) {
            cpuTScale.push(`T${i}`);
        }

        const cpuData: ICUPData = {
            CPUPLimit: data.PStateLimit,
            CPUTLimit: data.TStatelimit,
            CPUPState: data.Pstate,
            CPUTState: data.Tstate,
            CPUPScales: cpuPScale,
            CPUTScales: cpuTScale
        };
        return cpuData;
    }
}
