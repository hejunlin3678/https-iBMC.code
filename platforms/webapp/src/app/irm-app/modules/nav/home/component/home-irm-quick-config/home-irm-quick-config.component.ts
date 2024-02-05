import { Component, OnInit, Input } from '@angular/core';
import { StepsData } from '../../model';

@Component({
    selector: 'app-home-irm-quick-config',
    templateUrl: './home-irm-quick-config.component.html',
    styleUrls: ['./home-irm-quick-config.component.scss']
})
export class HomeIrmQuickConfigComponent implements OnInit {
    @Input() headerTitle: string = '';
    public aass;
    public bbcc;
    public stepsData = new StepsData();
    constructor() {
        this.aass = this.stepsData.getSteps;
        this.bbcc = this.stepsData.getActiveStep;
    }
    ngOnInit() {

    }
}
