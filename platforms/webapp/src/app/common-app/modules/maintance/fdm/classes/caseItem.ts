export class CaseItem {

    private no: number;
    private createTime: string;
    private type: string;
    private level: string;
    private info: string;

    constructor(
        no: number,
        type: string,
        level: string,
        createTime: string,
        info: string
    ) {
        this.no = no;
        this.type = type;
        this.level = level;
        this.createTime = createTime;
        this.info = info;
    }
    get getType () {
        return this.type;
    }
    get getLevel () {
        return this.level;
    }

    get getCreateTime () {
        return this.createTime;
    }
}
