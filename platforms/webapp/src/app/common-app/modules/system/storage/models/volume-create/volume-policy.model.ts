import { UntypedFormControl } from '@angular/forms';
import { StorageVaild } from '../../vaild/storage.vaild';
import { ISelect, IVolumePolicyData, RaidType } from '../storage-interface';
import { VolumeStaticModel } from '../static-data';
import { IOptions } from 'src/app/common-app/models';

export class VolumePolicyModel {
    private constructor() {
        this.selectData = {
            name: null,
            createCacheCadeFlag: false,
            optimumIOSizeBytes: null,
            defaultReadPolicy: null,
            defaultWritePolicy: null,
            defaultCachePolicy: null,
            driveCachePolicy: null,
            accessPolicy: null,
            initializationMode: null,
            cacheLineSize: null,
            associationLogical: null
        };
    }

    set setTwoCache(twoCache: boolean) {
        this.selectData.createCacheCadeFlag = twoCache;
    }
    get getTwoCache(): boolean {
        return this.selectData.createCacheCadeFlag;
    }

    set setStripeSize(stripeSize: ISelect) {
        this.stripeSize = stripeSize;
    }

    get getStripeSize(): ISelect {
        return this.stripeSize;
    }

    set setCacheLineSize(disable: boolean) {
        this.cacheLineSize.disable = disable;
    }

    get getCacheLineSize(): ISelect {
        return this.cacheLineSize;
    }

    set setReadPolicy(disable: boolean) {
        this.readPolicy.disable = disable;
    }

    get getReadPolicy(): ISelect {
        return this.readPolicy;
    }

    set setWritePolicy(disable: boolean) {
        this.writePolicy.disable = disable;
    }

    get getWritePolicy(): ISelect {
        return this.writePolicy;
    }

    set setIOPolicy(disable: boolean) {
        this.ioPolicy.disable = disable;
    }

    get getIOPolicy(): ISelect {
        return this.ioPolicy;
    }

    set setDriverCachePolicy(disable: boolean) {
        this.driverCachePolicy.disable = disable;
    }

    get getDriverCachePolicy(): ISelect {
        return this.driverCachePolicy;
    }

    set setAccessPolicy(disable: boolean) {
        this.accessPolicy.disable = disable;
    }

    get getAccessPolicy(): ISelect {
        return this.accessPolicy;
    }

    set setInitMode(disable: boolean) {
        this.initMode.disable = disable;
    }

    get getInitMode() {
        return this.initMode;
    }

    set setAccelerationMethod(disable: boolean) {
        this.accelerationMethod.disable = disable;
    }

    get getAccelerationMethod(): ISelect {
        return this.accelerationMethod;
    }

    set setAssociationLogical (disable: boolean) {
        this.associationLogical.disable = disable;
    }

    get getAssociationLogical (): ISelect {
        return this.associationLogical;
    }

    get getselectData(): IVolumePolicyData {
        return this.selectData;
    }

    private static instance: VolumePolicyModel;
    public selectData: IVolumePolicyData;
    private formContral: UntypedFormControl = new UntypedFormControl('', StorageVaild.assic15());
    // 条带大小
    private stripeSize: ISelect = {
        disable: false,
        options: []
    };
    // cacheLineSize
    private cacheLineSize: ISelect = {
        disable: false,
        options: VolumeStaticModel.cacheLineSize
    };
    // 读策略
    private readPolicy: ISelect = {
        disable: false,
        options: VolumeStaticModel.readPolicy
    };
    // 写策略
    private writePolicy: ISelect = {
        disable: false,
        options: VolumeStaticModel.writePolicy
    };
    // IO策略
    private ioPolicy: ISelect = {
        disable: false,
        options: VolumeStaticModel.ioPolicy
    };
    // 物理盘缓存策略
    private driverCachePolicy: ISelect = {
        disable: false,
        options: VolumeStaticModel.driverCachePolicy
    };
    // 访问策略
    private accessPolicy: ISelect = {
        disable: false,
        options: VolumeStaticModel.accessPolicy
    };
    // 初始化类型
    private initMode: ISelect = {
        disable: false,
        options: VolumeStaticModel.initMode
    };

    // 加速方法
    private accelerationMethod: ISelect = {
        disable: false,
        options: VolumeStaticModel.accelerationMethod
    };
    // 关联逻辑盘
    private associationLogical: ISelect = {
        disable: false,
        options: []
    };

    public static getInstance(): VolumePolicyModel {
        if (!VolumePolicyModel.instance) {
            VolumePolicyModel.instance = new VolumePolicyModel();
        }
        return VolumePolicyModel.instance;
    }


    public static initInstance(
        maxSize: number,
        minSize: number,
        readPolicy: boolean,
        writePolicy: boolean,
        ioPolicy: boolean,
        driveCachePolicy: boolean,
        accessPolicy: boolean,
        initMode: boolean,
        twoCache: boolean,
        raidType?: string,
        logicList?: IOptions[]
    ) {
        const selectData: any = {};

        selectData.name = null;
        selectData.createCacheCadeFlag = twoCache;


        // 设置条带大小
        VolumePolicyModel.getInstance().setStripeSize = {
            disable: twoCache,
            options: VolumeStaticModel.createStripeSize(maxSize, minSize)
        };
        // 设置cacheLineSize
        selectData.cacheLineSize = twoCache ? VolumePolicyModel.getInstance().getCacheLineSize.options[0] : null;
        VolumePolicyModel.getInstance().setCacheLineSize = false;
        const last = VolumePolicyModel.getInstance().getStripeSize.options.length - 1;

        // PMC/YZ卡默认都是256k,iMR卡支持64K,BCMR卡默认256KB,ARIES卡默认256KB
        const optionsItems = VolumePolicyModel.getInstance().getStripeSize.options;
        const optionsItemsIs256 = optionsItems.filter((item) => item.label === '256 KB');
        if (twoCache) {
            selectData.optimumIOSizeBytes = VolumePolicyModel.getInstance().getStripeSize.options[last];
        } else {
            // 数据为多个且存在256KB默认显示256选项
            if (optionsItems.length > 1 && optionsItemsIs256.length !== 0) {
                selectData.optimumIOSizeBytes =  optionsItemsIs256[0];
            } else {
                selectData.optimumIOSizeBytes = VolumePolicyModel.getInstance().getStripeSize.options[0];
            }
        }
        // 设置关联逻辑盘
        VolumePolicyModel.getInstance().getAssociationLogical.options = logicList;
        selectData.associationLogical = twoCache && raidType === RaidType.PMC ?
            VolumePolicyModel.getInstance().getAssociationLogical.options[0]  : null;
        // 设置读策略--二级缓存开启设置为null
        selectData.defaultReadPolicy = readPolicy ? null : (
            twoCache ?
                VolumePolicyModel.getInstance().getReadPolicy.options[0] :
                VolumePolicyModel.getInstance().getReadPolicy.options[1]
        );
        VolumePolicyModel.getInstance().setReadPolicy = twoCache ? true : readPolicy;

        // 设置写策略
        selectData.defaultWritePolicy = writePolicy ? null : VolumePolicyModel.getInstance().getWritePolicy.options[1];
        if (raidType === RaidType.ARIES) {
            // hi1880卡默认读、写策略差异化处理
            selectData.defaultReadPolicy = VolumePolicyModel.getInstance().getReadPolicy.options[0];
            selectData.defaultWritePolicy = VolumePolicyModel.getInstance().getWritePolicy.options[0];
        }
        VolumePolicyModel.getInstance().setWritePolicy = writePolicy;
        VolumePolicyModel.getInstance().writePolicy.options = twoCache ?
            [VolumeStaticModel.writePolicy[0], VolumeStaticModel.writePolicy[1]] : VolumeStaticModel.writePolicy;

        // 设置IO策略
        selectData.defaultCachePolicy = ioPolicy ? null : VolumePolicyModel.getInstance().getIOPolicy.options[0];
        VolumePolicyModel.getInstance().setIOPolicy = twoCache ? true : ioPolicy;

        // 设置物理盘缓存策略
        selectData.driveCachePolicy = driveCachePolicy ? null : VolumePolicyModel.getInstance().getDriverCachePolicy.options[0];
        VolumePolicyModel.getInstance().setDriverCachePolicy = twoCache ? true : driveCachePolicy;

        // 设置访问策略
        selectData.accessPolicy = accessPolicy ? null : VolumePolicyModel.getInstance().getAccessPolicy.options[0];
        VolumePolicyModel.getInstance().setAccessPolicy = twoCache ? true : accessPolicy;

        // 设置初始化类型
        selectData.initializationMode = raidType === RaidType.ARIES ? VolumeStaticModel.initMode[1] : VolumeStaticModel.initMode[0];
        VolumePolicyModel.getInstance().setInitMode = twoCache ? true : initMode;

        // 设置加速方法
        selectData.accelerationMethod = raidType === RaidType.PMC ? VolumeStaticModel.accelerationMethod[0] : null;
        VolumePolicyModel.getInstance().setAccelerationMethod = false;

        VolumePolicyModel.getInstance().selectData = selectData;

    }

    public static destroy(): void {
        if (VolumePolicyModel.instance) {
            VolumePolicyModel.instance = null;
        }
    }

    public getInitModeOptions(raidType?: string): IOptions[] {
        let optionsArr = [];
        switch (raidType) {
            case RaidType.PMC:
            case RaidType.HBA:
                optionsArr = optionsArr.concat( VolumeStaticModel.initMode.slice(0, 1), VolumeStaticModel.initMode.slice(3, 5));
                break;
            case RaidType.BRCM:
                optionsArr = VolumeStaticModel.initMode.slice(0, 3);
                break;
            case RaidType.ARIES:
                optionsArr = optionsArr.concat(VolumeStaticModel.initMode.slice(1, 2), VolumeStaticModel.initMode.slice(5));
                break;
            default:
                optionsArr = VolumeStaticModel.initMode.slice(0, 3);
        }
        return optionsArr;
    }

    public getInitPmcOptions(raidType?: string): IOptions[] {
      let optionsArr = [];
      if (raidType === RaidType.HBA) {
          optionsArr = VolumeStaticModel.accelerationMethod.filter(item => item.id !== 'ControllerCache');
      } else {
          optionsArr = VolumeStaticModel.accelerationMethod;
      }
      return optionsArr;
  }

    get getFormControl() {
        return this.formContral;
    }

    public checkSave(): boolean {
        if (this.formContral && (!this.formContral.valid && this.getTwoCache && this.getCacheLineSize || this.formContral.valid)) {
            return false;
        } else {
            return true;
        }
    }

}

