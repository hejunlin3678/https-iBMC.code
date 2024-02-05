export interface PowerTabs {
    id: string;
    title: string;
    active: boolean;
    onActiveChange?: any;
}

export interface DataDetail {
    Id?: string;
    slotId?: string;
    HotswapState?: number;
    powerFlat?: string;
    panel?: number;
    status?: string;
    image?: string;
}
export interface TipParams {
    id: string;
    type: any;
    title: string;
    content: string;
    okShow: boolean;
    cancalShow: boolean;
}
export enum ACTIONTYPE {
    ON = 'On',
    GRACEFULSHUTDOWN = 'GracefulShutdown',
    FORCERESTART = 'ForceRestart',
    FORCEOFF = 'ForceOff',
    FORCEPOWERCYCLE = 'ForcePowerCycle',
    NMI = 'Nmi'
}
export enum SOLTTYPE {
    Off = 1,
    PoweringOn,
    M3,
    On,
    PoweringOff,
    M6,
    M7
}
export enum DETAILTYPE {
    Off = 'POWERONOFF_BEEN_OFF powerOff',
    PoweringOn = 'POWERONOFF_BUSINESS_UP powerUp',
    M3 = 'POWERONOFF_BUSINESS_UP powerUp',
    On = 'POWERONOFF_BEEN_ON powerUp',
    PoweringOff = 'POWERONOFF_BUSINESS_DOWN powerOff',
    M6 = 'POWERONOFF_BUSINESS_DOWN powerOff',
    M7 = 'POWERONOFF_UNKOWN lostNetwork'
}


