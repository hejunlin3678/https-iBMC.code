export class DiskCharts {

    public partitionName: string;
    public totalCapacityGB: number;
    public usagePercent: number;
    public usedCapacityGB: number;

    constructor(partitionName: string, totalCapacityGB: number, usagePercent: number, usedCapacityGB: number) {
        this.partitionName = partitionName;
        this.totalCapacityGB = totalCapacityGB;
        this.usagePercent = usagePercent;
        this.usedCapacityGB = usedCapacityGB;
    }
}
