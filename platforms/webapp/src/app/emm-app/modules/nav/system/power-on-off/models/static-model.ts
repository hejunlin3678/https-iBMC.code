export class PowerOnOffModel {
    constructor() { }
    public btnList: any = [
        {
            id: 'On',
            btnName: 'POWER_ON',
            tab: [0, 1]
        },
        {
            id: 'GracefulShutdown',
            btnName: 'POWER_NORMAL_OFF',
            tab: [0, 1]
        },
        {
            id: 'ForceRestart',
            btnName: 'POWER_FORCED_RESTART',
            tab: [0, 1]
        },
        {
            id: 'ForceOff',
            btnName: 'POWER_STRONG_DOWN',
            tab: [0]
        },
        {
            id: 'ForcePowerCycle',
            btnName: 'POWER_FORCED_OFF_AND_ON',
            tab: [0]
        },
        {
            id: 'Nmi',
            btnName: 'NMI',
            tab: [0]
        }
    ];
    public nodeColumns = [
        {
            title: '',
            width: '5%'
        },
        {
            title: 'INFO_MAIN_BOARD_SLOT',
            width: '45%'
        },
        {
            title: 'STATUS',
            width: '50%'
        }
    ];
    public modulColumns = [
        {
            title: '',
            width: '5%'
        },
        {
            title: 'INFO_MAIN_BOARD_SLOT',
            width: '30%'
        },
        {
            title: 'POWERONOFF_POWER_FLAT',
            width: '30%'
        },
        {
            title: 'STATUS',
            width: '35%'
        }
    ];
}
