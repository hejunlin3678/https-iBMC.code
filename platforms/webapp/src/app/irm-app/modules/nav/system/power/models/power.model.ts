export class Power {
    powerConsumedWatts: string;
    remainCapacityWattHour: string;
    powerArr: Power[];

constructor(obj: {
    powerConsumedWatts,
    remainCapacityWattHour,
    powerArr
}) {
        this.powerConsumedWatts = obj.powerConsumedWatts;
        this.remainCapacityWattHour = obj.remainCapacityWattHour;
        this.powerArr = obj.powerArr;
    }
}
