export class NTPFunction {

    private enabled: boolean;
    private addressMode: string;
    private preferredServer: string;
    private alternateServer: string;
    private extraServer: string;
    private timeChange: boolean = false;
    private dateTime?: string;

    constructor(
        serviceEnabled: boolean,
        ntpAddressOrigin: string,
        preferredNtpServer: string,
        alternateNtpServer: string,
        extraNtpServer: string) {
        this.enabled = serviceEnabled;
        this.addressMode = ntpAddressOrigin;
        this.preferredServer = preferredNtpServer;
        this.alternateServer = alternateNtpServer;
        this.extraServer = extraNtpServer;
    }

    set setServiceEnabled(serviceEnabled: boolean) {
        this.enabled = serviceEnabled;
    }

    set setPreferredNtpServer(preferredNtpServer: string) {
        this.preferredServer = preferredNtpServer;
    }

    set setAlternateNtpServer(alternateNtpServer: string) {
        this.alternateServer = alternateNtpServer;
    }

    set setExtraNtpServer(extraNtpServer: string) {
        this.extraServer = extraNtpServer;
    }

    set setNtpAddressOrigin(ntpAddressOrigin: string) {
        this.addressMode = ntpAddressOrigin;
    }

    set setTimeChenge(timeChange: boolean) {
        this.timeChange = timeChange;
    }

    get getTimeChenge(): boolean {
        return this.timeChange;
    }

    set setDateTime(dataTime: string) {
        this.timeChange = false;
        this.dateTime = dataTime;
    }

}
