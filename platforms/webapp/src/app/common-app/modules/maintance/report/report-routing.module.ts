import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

import { ReportComponent } from './report.component';
import { SyslogComponent } from './syslog/syslog.component';
import { EmailComponent } from './email/email.component';
import { TrapComponent } from './trap/trap.component';

const routes: Routes = [
    {
        path: '',
        component: ReportComponent,
        children: [
            {
                path: 'syslog',
                component: SyslogComponent
            },
            {
                path: 'email',
                component: EmailComponent
            },
            {
                path: 'trap',
                component: TrapComponent
            },
            {
                path: '',
                pathMatch: 'full',
                redirectTo: 'syslog'
            }
        ]
    }
];

@NgModule({
    imports: [RouterModule.forChild(routes)],
    exports: [RouterModule]
})
export class ReportRoutingModule { }
