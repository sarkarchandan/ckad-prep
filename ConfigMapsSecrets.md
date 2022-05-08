# ConfigMaps and Secrets

A [ConfigMaps](https://kubernetes.io/docs/concepts/configuration/configmap/) is an API object used to store non-confidential data in key-value pairs. Pods can consume ConfigMaps as environment variables, command-line arguments using `kubectl` command, or as configuration files stored in a Volume. ConfigMap helps us to decouple environment-specific configuration from the container images, so that the applications are easily portable.

> It is important to take a note, that the ConfigMaps don't support secrecy / encryption. We should not use it store things like sensitive data or credentials. A dedicated K8s object called [Secret](https://kubernetes.io/docs/concepts/configuration/secret/) is there for that purpose.

The declarative approach to use a YAML for ConfigMaps is often referred to as a ConfigMap manifest. Following is an example of such a declaration. This can be used with K8s API using the usual commands like `k create -f <config_manifest_yaml> --save-config` or simply `k apply -f <config_manifest_yaml>`.

```yaml
apiVersion: v1
kind: ConfigMap # It is a COnfigMap kind of object
metadata:
  name: game-settings # Name metadata would be important as we'd see later, that we need it to get the configuration information
  labels:
    app: game-settings
data: # Data stores the actual configuration data, that we are interested in
  # Property-like keys; each key maps to a simple value
  player_initial_lives: "3"
  ui_properties_file_name: "user-interface.properties"

  # File-like keys
  game.properties: |
    enemy.types=aliens,monsters
    player.maximum-lives=5
  user-interface.properties: |
    color.good=purple
    color.bad=yellow
    allow.text_mode=true
```

Another approach to store configuration data is to use a config file kept in a Volume. This can be any file with name like game.cfg or game.config. This kind of configuration can also be used with K8s API like this, `k create configmap <config_map_name> --from-file=<config_file_path>`.

```config
enemies=aliens
lives=3
enemies.cheat=true
enemies.cheat.level=npGoodRotten
```

Using config file as above would lead to slightly different behavior than expected. It would lead K8s creating a manifest for us, which would look like below. Assuming, that we have mentioned the ConfigMap name as game-settings and the file name as game.config while invoking the command,

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: game-settings # Name is taken from the command line argument
data:
  game.config:
    | # This is interesting, as the file name becomes the key for the configuration
    enemies=aliens
    lives=3
    enemies.cheat=true
    enemies.cheat.level=npGoodRotten
```

Third option to create ConfigMap is to use a `.env` file. This is somewhat similar to, what we saw for the config file but the manifest it would create is slightly different than that of config file. The env file can look like below and the command we'd use for that is `k create configmap <config_map_name> --from-env-file=<env_file_path>`.

```env
<!-- As we see, the way to provide the key and values are exactly same as of config file -->
enemies=aliens
lives=3
enemies.cheat=true
enemies.cheat.level=npGoodRotten
```

The manifest, in which the env file would result in, looks like below.

```yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: game-settings # Name is taken from the command line argument
data: # The data would get the configuration key and values directly
  enemies=aliens
  lives=3
  enemies.cheat=true
  enemies.cheat.level=npGoodRotten
```

The final approach is to provide he configuration data as literals upon command line invocation. This may look somewhat like below. Usually this method is not preferred for the production deployment as we'd often like to store the configuration data in a version control somewhere. Hence other methods are often preferred. For testing in our dev environment we might make use of literal methods to create configurations on the go.

```bash
$ k create configmap <config_map_name> --from-literal=apiUrl=https://my-api --from-literal=otherKey=otherValue
```

Some useful commands to use with the ConfigMaps include,

- `k get cm` - In order to get all ConfigMaps
- `k get cm <config_map_name> -o yaml` - In order to see, what is stored inside the ConfigMap

Our next important aspect of using ConfigMap is, how to use them with the Containers. Let's see some examples for that.

> Using the ConfigMap created from an env file in a Container in the Pod manifest.

<img src="res/k8s21.png" width="1000" height="400" alt="Using config from env for specific">

There are few key things to take note here.

- In the Container specification of the Pod manifest we declare an `env` property
- The `env` property has a `name`, which becomes the name of an environment variable eventually.
- The `env` property has ta `valueFrom` property, which links a given ConfigMap manifest to the Pod manifest.
- The `valueFrom` then gets an underlying property called, `configMapKeyRef`, which encapsulates the details of how to get the value for a key and from where.
- We see, that the key reference has a `name` property having the name of a ConfigMap object and a `key` property, which refers to one of the keys mentioned under the `data` property of the ConfigMap object.
- Eventually, `ENEMIES` becomes one of the environment variables for the Container to use having value `aliens`.

This was an example to load a specific configuration as environment variable for the Container. Following is an example, where all configurations would be loaded as environment variables for the Container.

<img src="res/k8s22.png" width="1000" height="400" alt="Using config from env for all">

So in the above example we get four environment variables like, `ENEMIES`, `LIVES`, `ENEMIES_CHEAT`, and `ENEMIES_CHEAT_LEVEL`.

Another alternative to use ;configurations from ConfigMaps would be to use Volumes. Following is an example.

<img src="res/k8s23.png" width="1000" height="400" alt="Accessing config using Volume">

This leads to creating a individual files for each configuration under the `data` property in the manifest, in the mounted Volume. For instance there would be `enemies` file, `enemies.cheat` file etc. and the values in the files would be respective configuration values. The neat thing in this approach is, if the ConfigMap manifest changes for some reason, the changes reflect in the file, which are previously created inside the Volume automatically. There is however a delay involved with that. We also take notice, that we have `volume` property under the spec and inside there we have a `configMap` property, which links a given ConfigMap by `name`. Containers themselves use the `volumeMount` property in the usual manner.

Which is the preferred way here, depends upon, whether we are expecting the configuration values to change in runtime. If they remain static using them as environment variables makes sense. If not, we can take the alternative file approach.

Now we'd explore some examples for using ConfigMaps.

```bash

```

[Secret](https://kubernetes.io/docs/concepts/configuration/secret/)
