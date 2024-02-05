export interface IDetail {
    label: string;
    content: string;
    hide: boolean;
}
// 编辑字段
export enum AssetEditFormat {
    CHECKINTIIME = 'CheckInTime',
    UHEIGHT = 'UHeight',
    WEIGHTKG = 'WeightKg',
    LIFECYCLEYEAR = 'LifeCycleYear',
    RATEDPOWERWATTS = 'RatedPowerWatts'
}
