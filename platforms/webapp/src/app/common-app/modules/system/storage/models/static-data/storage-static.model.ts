import { IHealthState, IOptions } from 'src/app/common-app/models';

export class StorageStaticData {

    public static readonly isEmpty: string = '--';

    public static readonly stateArr: IHealthState[] = [
        {
            key: 'OK',
            label: 'STORE_NORMAL',
            className: 'eventLevelInformational',
            name: 'IHealthState'
        }, {
            key: 'Informational',
            label: 'STORE_NORMAL',
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

    // 一致性校验速率设置
    public static readonly checkRateOptions: IOptions[] = [
      {
          id: 'Low',
          label: 'Low',
          name: 'IOptions'
      },
      {
          id: 'Medium',
          label: 'Medium',
          name: 'IOptions'
      },
      {
          id: 'High',
          label: 'High',
          name: 'IOptions'
      },
  ];

    public static readonly writeCacheType: IOptions[] = [
        {
            id: 'ConfiguredDrive',
            label: 'ConfiguredDrive',
            name: 'IOptions'
        },
        {
            id: 'UnconfiguredDrive',
            label: 'UnconfiguredDrive',
            name: 'IOptions'
        },
        {
            id: 'HBADrive',
            label: 'HBADrive',
            name: 'IOptions'
        }
    ];

    public static readonly writeCachePolicy: IOptions[] = [
        {
            id: 'Enabled',
            label: 'Enabled',
            name: 'IOptions'
        },
        {
            id: 'Disabled',
            label: 'Disabled',
            name: 'IOptions'
        },
        {
            id: 'Default',
            label: 'Default',
            name: 'IOptions'
        }
    ];

    private constructor() {}

    public static getState(key: string): IHealthState {
        const state = StorageStaticData.stateArr.filter(
            (healthState) => healthState.key === key
        );
        return state[0] || null;
    }

    public static setPatrolState(key: string): string {
        let str = null;
        if (key === 'DoneOrNotPatrolled') {
            str = 'FDM_STOPPED';
        } else if (key === 'Patrolling') {
            str = 'FDM_CHECKING';
        }
        return str;
    }

    public static getPatrolState(key: string): string {
        let str = null;
        if (key === 'FDM_STOPPED') {
            str = 'DoneOrNotPatrolled';
        } else if (key === 'FDM_CHECKING') {
            str = 'Patrolling';
        }
        return str;
    }
}
