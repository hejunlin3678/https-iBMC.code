import { ITitle, Fans } from './fans.interface';

export class FansInfo {
    public fansList: Fans[];

    // 提供给html的TiTable
    private columns: ITitle[];

    public smartCoolingMode: string;
    public smartCoolingModeText: string;
    private activedCount: number;

    constructor(
        smartCoolingMode: string,
        smartCoolingModeText: string,
        activedCount: number,
        fansList: Fans[]
    ) {
        this.smartCoolingMode = smartCoolingMode;
        this.smartCoolingModeText = smartCoolingModeText;
        this.activedCount = activedCount;
        this.fansList = fansList;
    }

    get getColumns() {
        return this.columns;
    }
}
