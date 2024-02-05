import { IDetail } from './asset.datatype';


export class AssetStaticModel {

    private constructor() { }

    // 是否为启动盘
    public static readonly assetDetails: IDetail[] = [
        {
            label: 'ASSET_U_NUMBER',
            content: 'uNum',
            hide: false,
        },
        {
            label: 'IRM_DEVICE_MODEL',
            content: 'pattern',
            hide: false,
        },
        {
            label: 'COMMON_DEVICE_TYPE',
            content: 'deviceType',
            hide: false,
        },
        {
            label: 'COMMON_MANUFACTURER',
            content: 'factory',
            hide: false,
        },
        {
            label: 'IRM_POSITION_USE',
            content: 'uHeight',
            hide: false,
        },
        {
            label: 'COMMON_SERIALNUMBER',
            content: 'serialNumber',
            hide: false,
        },
        {
            label: 'IRM_PART_NUMBER',
            content: 'partNumber',
            hide: false,
        },
        {
            label: 'ASSET_LIFE_CYCLE',
            content: 'lifeCircle',
            hide: false,
        },
        {
            label: 'IRM_DISCOVER_TIME',
            content: 'shelfTime',
            hide: false,
        },
        {
            label: 'IRM_CHECKIN_TIME',
            content: 'startTime',
            hide: false,
        },
        {
            label: 'ASSET_WEIGHT',
            content: 'weight',
            hide: false,
        },
        {
            label: 'ASSET_RATED_POWER_WATTS',
            content: 'ratedPower',
            hide: false,
        },
        {
            label: 'IRM_ASSET_OWNER',
            content: 'owner',
            hide: false,
        },
    ];
}
