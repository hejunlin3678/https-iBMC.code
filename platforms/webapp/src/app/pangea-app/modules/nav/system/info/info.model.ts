
import { ITabTitle } from './info.interface';
import { Pangen } from 'src/app/pangea-app/models/pangen.model';
import { OceanType } from 'src/app/pangea-app/models/common.datatype';

export class Info {

    private constructor() {}

    private static arcticInfoTitle: ITabTitle[] = [
        {
            title: 'SYSINFO_PRODUCTINFO',
            isActived: true,
            path: '/navigate/system/info/product',
            id: 'systemInfoProduct'
        },
        {
            title: 'HOME_PROCESSOR',
            isActived: false,
            path: '/navigate/system/info/processor',
            id: 'systemInfoProcessor'
        },
        {
            title: 'SYSINFO_MEMORY',
            isActived: false,
            path: '/navigate/system/info/memory',
            id: 'systemInfoMemory'
        },
        {
            title: 'COMMON_NETWORK',
            isActived: false,
            path: '/navigate/system/info/net',
            id: 'systemInfoNic'
        },
        {
            title: 'SYSINFO_SENSOR',
            isActived: false,
            path: '/navigate/system/info/sensor',
            id: 'systemInfoSensor'
        },
        {
            title: 'SYSINFO_ONTERS',
            isActived: false,
            path: '/navigate/system/info/others',
            id: 'systemInfoOthers'
        }
    ];

    private static atlanticInfoTitle: ITabTitle[] = [
        {
            title: 'SYSINFO_PRODUCTINFO',
            isActived: true,
            path: '/navigate/system/info/product',
            id: 'systemInfoProduct'
        },
        {
            title: 'HOME_PROCESSOR',
            isActived: false,
            path: '/navigate/system/info/processor',
            id: 'systemInfoProcessor'
        },
        {
            title: 'SYSINFO_MEMORY',
            isActived: false,
            path: '/navigate/system/info/memory',
            id: 'systemInfoMemory'
        },
        {
            title: 'COMMON_NETWORK',
            isActived: false,
            path: '/navigate/system/info/net',
            id: 'systemInfoNic'
        },
        {
            title: 'SYSINFO_SENSOR',
            isActived: false,
            path: '/navigate/system/info/sensor',
            id: 'systemInfoSensor'
        },
        {
            title: 'SYSINFO_ONTERS',
            isActived: false,
            path: '/navigate/system/info/others',
            id: 'systemInfoOthers'
        }
    ];

    private static atlanticSMMInfoTitle: ITabTitle[] = [
        {
            title: 'SYSINFO_PRODUCTINFO',
            isActived: true,
            path: '/navigate/system/info/product',
            id: 'systemInfoProduct'
        },
        {
            title: 'SYSINFO_SENSOR',
            isActived: false,
            path: '/navigate/system/info/sensor',
            id: 'systemInfoSensor'
        },
    ];

    private static pacificInfoTitle: ITabTitle[] = [
        {
            title: 'SYSINFO_PRODUCTINFO',
            isActived: true,
            path: '/navigate/system/info/product',
            id: 'systemInfoProduct'
        },
        {
            title: 'HOME_PROCESSOR',
            isActived: false,
            path: '/navigate/system/info/processor',
            id: 'systemInfoProcessor'
        },
        {
            title: 'SYSINFO_MEMORY',
            isActived: false,
            path: '/navigate/system/info/memory',
            id: 'systemInfoMemory'
        },
        {
            title: 'COMMON_NETWORK',
            isActived: false,
            path: '/navigate/system/info/net',
            id: 'systemInfoNic'
        },
        {
            title: 'SYSINFO_SENSOR',
            isActived: false,
            path: '/navigate/system/info/sensor',
            id: 'systemInfoSensor'
        },
        {
            title: 'SYSINFO_ONTERS',
            isActived: false,
            path: '/navigate/system/info/others',
            id: 'systemInfoOthers'
        }
    ];

    public static getInfoTitle(): ITabTitle[] {
        let titles: ITabTitle[] = [];
        switch (Pangen.ocean) {
            case OceanType.Arctic:
                titles = Info.arcticInfoTitle;
                break;
            case OceanType.Atlantic:
                titles = Info.atlanticInfoTitle;
                break;
            case OceanType.AtlanticSMM:
                titles = Info.atlanticSMMInfoTitle;
                break;
            case OceanType.Pacific:
                titles = Info.pacificInfoTitle;
                break;
            default:
                titles = Info.arcticInfoTitle;
                break;
        }
        return titles;
    }

}
