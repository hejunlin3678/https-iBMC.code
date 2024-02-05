import { VolumesComponent } from './volumes/volumes.component';
import { SpanComponent } from './span/span.component';
import { RaidComponent } from './raid/raid.component';
import { DriversComponent } from './drivers/drivers.component';
import { RaidEditComponent } from './raid/raid-edit/raid-edit.component';
import { DriverEditComponent } from './drivers/driver-edit/driver-edit.component';
import { VolumnEditComponent } from './volumes/volumn-edit/volumn-edit.component';
import { VolumePolicyComponent } from './volume-create/components/volume-policy/volume-policy.component';
import { VolumeMembersComponent } from './volume-create/components/volume-members/volume-members.component';
import { VolumeCreateComponent } from './volume-create/volume-create.component';
import { OnekeyCollectComponent } from './raid/onekey-collect/onekey-collect.component';
import { VolumnPMCEditComponent } from './volumes/volumn-pmc-edit/volumn-pmc-edit.component';
import { VolumnAriesEditComponent } from './volumes/volumn-aries-edit/volumn-aries-edit.component';


const dynamicComponent = {
    VolumeNode: VolumesComponent,
    SpanNode: SpanComponent,
    RaidNode: RaidComponent,
    DriverNode: DriversComponent,
};

const importComponents = [
    VolumesComponent,
    SpanComponent,
    RaidComponent,
    DriversComponent,
    RaidEditComponent,
    DriverEditComponent,
    VolumnEditComponent,
    VolumnPMCEditComponent,
    VolumnAriesEditComponent,
    DriverEditComponent,
    VolumePolicyComponent,
    VolumeMembersComponent,
    VolumeCreateComponent,
    OnekeyCollectComponent
];

export { dynamicComponent, importComponents };
