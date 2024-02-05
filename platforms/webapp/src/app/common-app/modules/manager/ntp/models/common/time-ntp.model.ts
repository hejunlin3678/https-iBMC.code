import { NTPFunction } from '../ntp-function/ntp-function.model';
import { Polling } from '../polling/polling.model';
import { TimeArea } from '../time-area/time-area.model';
import { Authen } from '../authen/authen.model';

export class TimeNTP {
    private timeArea: TimeArea;
    private ntpFunction: NTPFunction;
    private polling: Polling;
    private authen: Authen;
    private dstEnabled: boolean;

    get getTimeArea(): TimeArea {
        return this.timeArea;
    }

    get getNTPFunction(): NTPFunction {
        return this.ntpFunction;
    }

    get getPolling(): Polling {
        return this.polling;
    }

    get getAuthen(): Authen {
        return this.authen;
    }

    get getDstEnabled(): boolean {
        return this.dstEnabled;
    }

    set setTimeArea(timeArea: TimeArea) {
        this.timeArea = timeArea;
    }

    set setNtpFunction(ntpFunction: NTPFunction) {
        this.ntpFunction = ntpFunction;
    }

    set setPolling(polling: Polling) {
        this.polling = polling;
    }

    set setAuthen(authen: Authen) {
        this.authen = authen;
    }

    set setDstEnabled(dstEnabled: boolean) {
        this.dstEnabled = dstEnabled;
    }

}
