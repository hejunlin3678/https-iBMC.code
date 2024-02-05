import { Card } from '../card';
import { DiskBackPlane } from './disk-back-plane';

export class DiskBackPlaneArray extends Card {
    private diskBackPlanes: DiskBackPlane[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_NAME',
                width: '10%',
                show: true
            },
            {
                title: 'OTHER_POSITION',
                width: '10%',
                show: true
            },
            {
                title: 'COMMON_MANUFACTURER',
                width: '10%',
                show: true
            },
            {
                title: 'COMMON_NUMBER',
                width: '5%',
                show: true
            },
            {
                title: 'COMMON_TYPE',
                width: '15%',
                show: true
            },
            {
                title: 'COMMON_ALARM_PCB_VERSION',
                width: '8%',
                show: true
            },
            {
                title: 'COMMIN_CPLD_VERSION',
                width: '10%',
                show: true
            },
            {
                title: 'OTHER_BOARD_ID',
                width: '8%',
                show: true
            },
            {
                title: 'OTHER_PART_NUMBER',
                width: '10%',
                show: true
            },
            {
                title: 'COMMON_SERIALNUMBER',
                width: '14%',
                show: true
            }];
        this.diskBackPlanes = [];
    }

    set setDiskBackPlanes(diskBackPlanes: DiskBackPlane[]) {
        this.diskBackPlanes = diskBackPlanes;
    }

    get getDiskBackPlanes(): DiskBackPlane[] {
        return this.diskBackPlanes;
    }

    get getTitle(): object[] {
        return this.title;
    }

    addDiskBackPlanes(diskBackPlane: DiskBackPlane): void {
        this.diskBackPlanes.push(diskBackPlane);
    }

}
