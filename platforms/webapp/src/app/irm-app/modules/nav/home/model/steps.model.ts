export class StepsData {
    private steps;
    private activeStep;
    constructor () {
        this.steps = [
            {label: ''},
            {label: ''},
            {label: ''},
            {label: ''}
        ];
        this.activeStep = this.steps[0];
    }
    get getSteps () {
        return this.steps;
    }
    get getActiveStep () {
        return this.activeStep;
    }
    set setActiveStrp (param) {
        this.activeStep = param;
    }
}
