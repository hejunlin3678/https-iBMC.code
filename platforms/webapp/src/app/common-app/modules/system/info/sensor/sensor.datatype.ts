export interface ISensorTableRow {
    id: number;
    name: string;
    status: statusList;
    statusText: string;
    statusClassName: string;
    statusIcon: string;
    readingValue: string;
    lowerThresholdCritical: string;
    lowerThresholdFatal: string;
    lowerThresholdNonCritical: string;
    upperThresholdNonCritical: string;
    upperThresholdCritical: string;
    upperThresholdFatal: string;
}

export interface IStatusObj {
    className: string;
    text: string;
    icon: string;
}

export enum sensorUnit {
    RPM = 'RPM',
    Volts = 'V',
    'degrees C' = '℃',
    Watts = 'W',
    Amps = 'A',
}

export enum statusList {
    ok = 'ok',
    nc = 'nc',
    cr = 'cr',
    nr = 'nr',
}
