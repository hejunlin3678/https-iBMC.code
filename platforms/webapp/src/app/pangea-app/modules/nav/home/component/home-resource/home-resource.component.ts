import { Router } from '@angular/router';
import { Component, Input } from '@angular/core';
import { IResource } from '../../home.datatype';

@Component({
    selector: 'app-home-resource',
    templateUrl: './home-resource.component.html',
    styleUrls: ['./home-resource.component.scss']
})
export class HomeResourceComponent {

    @Input() resource: IResource;
    constructor(private router: Router) { }

    public clickFn() {
        this.router.navigate(this.resource.targetNav);
    }

}
