export class BBUModel {
    private name: [string, string];
    private state: [string, string];
    private healthState: [string, string];

    constructor(
        name: string,
        state: string,
        healthState: string
    ) {
        this.name = ['COMMON_NAME', name];
        this.state = ['STATUS' , state];
        this.healthState = ['NET_HEALTH_STATUS', healthState];
    }
}
