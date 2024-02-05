import { Router } from '@angular/router';
import { Component, OnInit, Input } from '@angular/core';
import { IResource } from '../../home.datatype';

@Component({
    selector: 'app-home-emm-resource',
    templateUrl: './home-resource.component.html',
    styleUrls: ['./home-resource.component.scss']
})
export class HomeEmmResourceComponent implements OnInit {

    @Input() resource: IResource;

    constructor(private router: Router) { }

    ngOnInit() {
    }

    public clickFn() {
        this.router.navigate(this.resource.targetNav);
    }

}
