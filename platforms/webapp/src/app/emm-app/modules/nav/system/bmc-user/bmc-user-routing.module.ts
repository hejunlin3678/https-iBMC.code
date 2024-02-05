import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { TranslateModule } from '@ngx-translate/core';
import { UsersComponent } from './components/users.component';

const routes: Routes = [
    {
        path: '',
        component: UsersComponent
    }
];

@NgModule({
    imports: [
        TranslateModule,
        RouterModule.forChild(routes)
    ],
    exports: [RouterModule]
})
export class BmcUserRoutingModule { }
