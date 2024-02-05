import { IHealthState } from 'src/app/common-app/models/common.interface';

export class AlarmConfigState {
    private constructor() { }
    public static readonly stateArr: IHealthState[] = [
        {
            key: 'Normal',
            label: 'ALARM_EVENT_NORMAL',
            className: 'eventLevelInformational',
            name: 'IHealthState'
        }, {
            key: 'Minor',
            label: 'ALARM_EVENT_MINOR',
            className: 'eventLevelMinor',
            name: 'IHealthState'
        }, {
            key: 'Major',
            label: 'ALARM_EVENT_MAJOR',
            className: 'eventLevelMajor',
            name: 'IHealthState'
        }, {
            key: 'Critical',
            label: 'ALARM_EVENT_CRITICAL',
            className: 'eventLevelCritical',
            name: 'IHealthState'
        }
    ];

    public static getState(key: string): IHealthState {
        const state = AlarmConfigState.stateArr.filter(
            (healthState) => healthState.key === key
        );
        return state[0] || null;
    }
}
