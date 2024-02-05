export class Email {
    private num: string | number;
    private address: string;
    private description: string;
    private emailEnable: boolean;
    constructor(num, address, description, emailEnable) {
        this.num = num;
        this.address = address;
        this.description = description;
        this.emailEnable = emailEnable;
    }
}
