import { IPolling } from '../common/ntp.interface';
import { IReturnVal } from 'src/app/common-app/models';

export class PollingData {

    private pollingList: IPolling[];
    private minSelect: IPolling;
    private maxSelect: IPolling;
    private pollingDisable: boolean;

    constructor() {
        this.pollingList = [];
        for (let i = 0; i < 15; i++) {
            this.pollingList.push({
                id: i + 3,
                label: 'NTP_TIME' + i
            });
        }
    }

    get getPollingList(): IPolling[] {
        return this.pollingList;
    }

    get getMinSelect(): IPolling {
        return this.minSelect;
    }

    get getMaxSelect(): IPolling {
        return this.maxSelect;
    }

    public setMinSelect(minSelect: IPolling): IReturnVal {
        this.minSelect = minSelect;
        if (this.maxSelect && minSelect.id > this.maxSelect.id) {
            return {state: false, label: 'NTP_TIME_TIPS_MIN'};
        }
        return {state: true, label: 'COMMON_SUCCESS'};
    }

    public setMaxSelect(maxSelect: IPolling): IReturnVal {
        this.maxSelect = maxSelect;
        if (this.minSelect && maxSelect.id < this.minSelect.id ) {
            return {state: false, label: 'NTP_TIME_TIPS_MAX'};
        }
        return {state: true, label: 'COMMON_SUCCESS'};
    }

    public setPollingDisable(privilege: boolean): void {
        this.pollingDisable = !privilege;
    }

    get getPollingDisable(): boolean {
        return this.pollingDisable;
    }
}
