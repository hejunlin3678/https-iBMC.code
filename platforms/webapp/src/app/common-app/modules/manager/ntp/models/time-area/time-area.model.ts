export class TimeArea {
    // 地区和时区,第一个是时区，第二个是地区
    private time: string;
    private area: string;

    set setArea(area: string) {
        this.area = area;
    }

    set setTime(time: string) {
        this.time = time;
    }

    get getTime(): string {
        return this.time;
    }

    get getArea(): string {
        return this.area;
    }
}
