import { TimeAreaComponent } from '../../time-area/time-area.component';
import { NTPFunctionComponent } from '../../ntp-function/ntp-function.component';
import { PollingComponent } from '../../polling/polling.component';
import { TiValidationConfig } from '@cloud/tiny3';
import { NtpAuthenComponent } from '../../ntp-authen/ntp-authen.component';

export interface INTPLoad {
    timeArea: boolean;
    ntpFunction: boolean;
    polling: boolean;
    authen: boolean;
}

export interface IZreo {
    key: string;
    label: string;
}

export interface IParam {
    content: string;
    value: string;
}

export interface IComponents {
    timeAreaComponent: TimeAreaComponent;
    ntpFunctionComponent: NTPFunctionComponent;
    pollingComponent: PollingComponent;
    ntpAuthenComponent: NtpAuthenComponent;
}

export interface IPolling {
    id: number;
    label: string;
}

export interface IVerifica {
    servesPref: boolean;
    servesAlte: boolean;
    polling: boolean;
}

export interface IRadio {
    key: string;
    id: string;
    disable: boolean;
}

export interface INtpModal {
    server: INtp;
    serverPreferred: INtp;
    serverAlternate: INtp;
}

export interface INtp {
    id: string;
    label: string;
    value: string;
    count: number;
    tip: TiValidationConfig;
}

export interface IHeaders {
    From: string;
    token: string;
}
