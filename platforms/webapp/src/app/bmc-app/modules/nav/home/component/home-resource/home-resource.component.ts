import { Router } from '@angular/router';
import { Component, OnInit, Input } from '@angular/core';
import { IResource } from '../../home.datatype';
import { GlobalData } from 'src/app/common-app/models/global-data.model';

@Component({
    selector: 'app-home-resource',
    templateUrl: './home-resource.component.html',
    styleUrls: ['./home-resource.component.scss']
})
export class HomeResourceComponent implements OnInit {

    @Input() resource: IResource;
    public webStyle: string;
    public shenmaStyle: boolean;
    constructor(private router: Router) { }

    ngOnInit() {
        this.webStyle = GlobalData.getInstance().getWebStyle;
        this.shenmaStyle = GlobalData.getInstance().getShenmaStyle;
    }

    public clickFn() {
        this.router.navigate(this.resource.targetNav);
    }

}
