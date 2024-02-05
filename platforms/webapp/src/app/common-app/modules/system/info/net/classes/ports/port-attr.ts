
export abstract class PortArr {

    titles: IPortThs[];
    netPorts: Port[];

    abstract  addPort(port: Port): void;
}

// tslint:disable-next-line: max-classes-per-file
export abstract class Port {
    extendAttr: ExtendAttr;
    constructor(extendAttr: ExtendAttr) {
        this.extendAttr = extendAttr;
    }
}

// tslint:disable-next-line: max-classes-per-file
export abstract class ExtendAttr {}

interface IPortThs {
    title: string;
    width: string;
}
