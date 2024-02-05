export class Ports {
    private title: string;
    private tabThs: ITabTh[];
    private portArray: IPort[];
    private showPort: boolean;

    constructor(portArray: IPort[]) {
        this.title = 'OTHER_PORT_ATTR';
        this.tabThs = [
            {
                title: 'SERVICE_PORT'
            }, {
                title: 'OTHER_PFID'
            }, {
                title: 'OTHER_PERMANENT_MACADDR'
            }
        ];
        this.portArray = portArray;
        this.showPort = false;
    }

    set setShowPort(showPort: boolean) {
        this.showPort = showPort;
    }

    get getShowPort(): boolean {
        return this.showPort;
    }
}

interface ITabTh {
    title: string;
}

export interface IPort {
    Port: string;
    PfId: string;
    PermanentMac: string;
}
