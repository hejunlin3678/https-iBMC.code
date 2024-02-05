export class ThresholdAttr {
    public temperature = null;
    public opticalAlarmT = null;
    public voltage = null;
    public opticalAlarmV = null;
    constructor(
        temperature: string,
        voltage: string,
        opticalAlarmT: string,
        opticalAlarmV: string,
        data?: any
    ) {
        this.temperature = ['CURRENT_TEMPERTURE', temperature];
        this.voltage = ['CURRENT_VOLTAGE', voltage];
        this.opticalAlarmT = ['OPTICAL_ALARM', opticalAlarmT];
        this.opticalAlarmV = ['OPTICAL_ALARM', opticalAlarmV];
        for (const key in data) {
            if (data.hasOwnProperty(key)) {
                const value = data[key];
                this[key] = [this._filterEntries(key), value];
            }
        }
    }

    private _filterEntries(key) {
        const reg = /^(OPTICAL_ALARM)+/ig;
        if (reg.test(key)) {
            return 'OPTICAL_ALARM';
        } else {
            return key;
        }
    }
}
