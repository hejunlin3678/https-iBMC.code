import { BackupBatteryUnits } from './backup-battery-units';
import { Card } from '../card';

export class BackupBatteryUnitsArray extends Card {

    private backupBatteryUnits: BackupBatteryUnits[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                width: '10%'
            },
            {
                title: 'OTHER_ID',
                width: '5%'
            },
            {
                title: 'COMMON_FIXED_VERSION',
                width: '8%'
            },
            {
                title: 'OTHER_WORK_STATUS',
                width: '8%'
            },
            {
                title: 'OTHER_CAPATICY',
                width: '10%'
            },
            {
                title: 'OTHER_BATTERY_MODEL',
                width: '8%'
            },
            {
                title: 'COMMON_SERIALNUMBER',
                width: '12%'
            },
            {
                title: 'COMMON_MANUFACTURER',
                width: '10%'
            }, {
                title: 'OTHER_M2_INPOSITION_1_2',
                width: '10%'
            }
        ];

        this.backupBatteryUnits = [];
    }

    get getBackupBatteryUnits(): BackupBatteryUnits[] {
        return this.backupBatteryUnits;
    }

    addBackupBatteryUnits(backupBatteryUnits: BackupBatteryUnits) {
        this.backupBatteryUnits.push(backupBatteryUnits);
    }
}
