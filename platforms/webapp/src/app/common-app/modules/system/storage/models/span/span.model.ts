export class Span {

    private name: [string , string];
    private usedSpace: [string, string];
    private remainingSpace: [string , string];

    constructor(
        name: string,
        usedSpace: string,
        remainingSpace: string
    ) {
        this.name = ['STORE_DRIVER_Group_ID', name];
        this.remainingSpace = ['STORE_REMAIN_SPACE', remainingSpace];
        this.usedSpace = ['STORE_USED_SPACE', usedSpace];
    }

}
