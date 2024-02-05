export class ConnectView {
    public switchName: [string, string];
    public switchConnectionID: [string, string];
    public switchConnectionPortID: [string, string];
    public switchPortVlanID: [string, string];
    constructor(
        switchName: string,
        switchConnectionID: string,
        switchConnectionPortID: string,
        switchPortVlanID: string
    ) {
        this.switchName = ['NET_SWITCH_NAME', switchName];
        this.switchConnectionID = ['NET_SWITCH_ID', switchConnectionID];
        this.switchConnectionPortID = ['NET_SWITCH_PORT_ID', switchConnectionPortID];
        this.switchPortVlanID = ['NET_SWITCH_VLAN_ID', switchPortVlanID];
    }
}
