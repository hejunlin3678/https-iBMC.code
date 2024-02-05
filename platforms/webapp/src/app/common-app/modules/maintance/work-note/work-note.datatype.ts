export interface IRecord {
    id: string;
    user: string;
    ip: string;
    time: string;
    details: string;
}

export interface IHalfModalContent {
    currentIP: string;
    userName: string;
    systemLockDownEnabled: boolean;
}

export interface IAddRecord {
    Content: string;
}

export interface IEditRecord {
    ID: string;
    Content: string;
}
