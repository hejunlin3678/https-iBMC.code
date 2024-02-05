export class Battery {
    private ratedCapacityWattHour: string;
    private remainCapacityWattHour: string;
    private batteryArr: Battery[];

    constructor(obj: {
        ratedCapacityWattHour,
        remainCapacityWattHour,
        batteryArr
    }) {
        this.ratedCapacityWattHour = obj.ratedCapacityWattHour;
        this.remainCapacityWattHour = obj.remainCapacityWattHour;
        this.batteryArr = obj.batteryArr;
    }
}
