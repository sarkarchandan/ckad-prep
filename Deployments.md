# Deployments

We need to understand about ReplicaSets before we talk about the Deployments. ReplicaSets are the fundamental K8s object, which maintains the creation and managing the Pods. A Deployment manages the Pods with the help of ReplicaSets. In the evolution story of K8s ReplicaSets came before Deployments. One of the basic promises of K8s is to preserve the desired cluster state. The applications are implemented and running inside the containers and they are hosted by the Pods. Hence, in some sense at any given point in time the running Pods and their number inside the cluster represent the cluster state. Deployment and ReplicaSets are the mechanism, with which this is state is maintained.

Deployments and ReplicaSets implement the self-healing mechanism of K8s. At any given point they maintain the desired number of running Pods. We'd see, that they also depend upon the Pod template. Following are some key points to keep in mind, before we start going into practical handling of Deployments.

- Deployments can scale Pods by adapting the ReplicaSets.
- Deployments and ReplicaSets support zero-downtime deployment of applications i.e., in order to rollout a new cluster state we don't need to take down the application.
- They also enable the quicker rollback utility.
- Previously, we used some `labels` for the Pods but did not make use of them anywhere. We'd shortly see, that the Deployments and ReplicaSets make use of them to manage Pods.

Following is an example YAML specification for a Deployment object.

```yaml
apiVersion: apps/v1 # Specify the API version, this time it is a bit different from Pod
kind: Deployment # Kind of the object is Deployment
metadata: # Place to put useful information about the Deployment like its name and any labels
  name: nginx-deployment # Name of the deployment
  labels: # Label specification for the Deployment. If we have multiple Deployments, we can select a particular one with Deployment. Later we'd see, where this is helpful
    app: my-nginx
    tier: frontend
spec: # Deployment specification
  selector: # Pod selector (encapsulates the mechanism to select Pods that it must manage)
    matchLabels: # We specify, that we want to select Pods, which match the specified labels
      app: nginx
  replicas: 2 # Tells deployment to run 2 pods matching the template (in our case template the is matchLabels defined under the selector)
  template: # Template used to create Pods
    metadata: # Place the useful information about the Pod like specify its labels, based on which it would be selected by the Deployment
      labels: # Labels specification
        app: nginx # Label based on which this Pod would be identified by the Deployment
    spec: # Pod specification
      containers: # Container specification
        - name: nginx # Container name
          image: nginx:1.14.2 # Base image for the container
          ports: # Port specification
            - containerPort: 80 # Container pot
```

The example above is one, in which we have included the Pod template inside the Deployment. That may not be the desirable way to go always. If we want, we could also define Pod templates in separate files and mention them under template. But this style of putting it together is also popular to have a full view of the desired cluster state. In a Deployment specification, following fields are expected.

- `apiVersion` - Which version of the K8s API to use.
- `kind` - What kind of object we want to create.
- `metadata` - Data that helps uniquely identify the object, including a name string, UID, and optional namespace.
- `spec` - What state we desire for the object and that refers to the first level spec, that we see in our example above.

Following are some of the commands, which we'd make use of in he following practice exercises.

```bash
$ k create -f deployment_spec.yaml --save-config
$ k apply -f deployment_spec.yaml
$ k get deployment --show-labels
$ k get deployment -l app=nginx # -l flag is used to filter deployments with label e.g., app:my-nginx
$ k delete deployment <deployment_name>
$ k delete deployment -f deployment_spec.yaml
$ k scale deployment [<deployment_name> | -f deployment_spec.yaml] --replicas=5 # We could also do this by simply updating the YAML specification and use the apply command
```

Now it is time to see some of these commands in action. We have created the following Deployment specification.

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: my-nginx
  labels:
    app: my-nginx
spec:
  selector:
    matchLabels:
      app: my-nginx
  replicas: 2
  template:
    metadata:
      labels:
        app: my-nginx
    spec:
      containers:
        - name: my-nginx
          image: nginx:alpine
          resources:
            limits:
              memory: "128Mi"
              cpu: "200m"
          ports:
            - containerPort: 80
          livenessProbe:
            httpGet:
              path: /index.html
              port: 80
            initialDelaySeconds: 15
            timeoutSeconds: 2
            periodSeconds: 5
            failureThreshold: 1
          readinessProbe:
            httpGet:
              path: /index.html
              port: 80
            initialDelaySeconds: 3
            periodSeconds: 5
            failureThreshold: 1
```

and with this yaml specification we are going to create a deployment.

```bash
$ k create -f fundamentals/Deployment/nginx_deployment_spec.yaml --dry-run=client --validate=true

deployment.apps/my-nginx created (dry run)

$ k create -f fundamentals/Deployment/nginx_deployment_spec.yaml --save-config
deployment.apps/my-nginx created

$ k get all
NAME                            READY   STATUS    RESTARTS   AGE
pod/my-nginx-54cddf7468-9qv6b   1/1     Running   0          18s
pod/my-nginx-54cddf7468-jl5lt   1/1     Running   0          18s

NAME                 TYPE        CLUSTER-IP   EXTERNAL-IP   PORT(S)   AGE
service/kubernetes   ClusterIP   10.96.0.1    <none>        443/TCP   20d

NAME                       READY   UP-TO-DATE   AVAILABLE   AGE
deployment.apps/my-nginx   2/2     2            2           18s

NAME                                  DESIRED   CURRENT   READY   AGE
replicaset.apps/my-nginx-54cddf7468   2         2         2       18s

$ k get deploy # Using any of the deploy, deployment or deployments have the same effect
NAME       READY   UP-TO-DATE   AVAILABLE   AGE
my-nginx   2/2     2            2           105s

$ k get deploy --show-labels
NAME       READY   UP-TO-DATE   AVAILABLE   AGE     LABELS
my-nginx   2/2     2            2           6m36s   app=my-nginx

$ k describe deployment -l app=my-nginx # Here is an example where we are using the label selector instead of the name
Name:                   my-nginx
Namespace:              default
CreationTimestamp:      Tue, 03 May 2022 04:43:09 +0200
Labels:                 app=my-nginx
Annotations:            deployment.kubernetes.io/revision: 1
Selector:               app=my-nginx
Replicas:               2 desired | 2 updated | 2 total | 2 available | 0 unavailable
StrategyType:           RollingUpdate
MinReadySeconds:        0
RollingUpdateStrategy:  25% max unavailable, 25% max surge
Pod Template:
  Labels:  app=my-nginx
  Containers:
   my-nginx:
    Image:      nginx:alpine
    Port:       80/TCP
    Host Port:  0/TCP
    Limits:
      cpu:        200m
      memory:     128Mi
    Liveness:     http-get http://:80/index.html delay=15s timeout=2s period=5s #success=1 #failure=1
    Readiness:    http-get http://:80/index.html delay=3s timeout=1s period=5s #success=1 #failure=1
    Environment:  <none>
    Mounts:       <none>
  Volumes:        <none>
Conditions:
  Type           Status  Reason
  ----           ------  ------
  Available      True    MinimumReplicasAvailable
  Progressing    True    NewReplicaSetAvailable
OldReplicaSets:  <none>
NewReplicaSet:   my-nginx-54cddf7468 (2/2 replicas created)
Events: # As we see, in this case the nature of the events are a bit different from that of the Pods
  Type    Reason             Age    From                   Message
  ----    ------             ----   ----                   -------
  Normal  ScalingReplicaSet  3m19s  deployment-controller  Scaled up replica set my-nginx-54cddf7468 to 2

$ k get po # This shows the number of currently running Pods. We want to scale this to 3 from command line
NAME                        READY   STATUS    RESTARTS   AGE
my-nginx-54cddf7468-9qv6b   1/1     Running   0          9m11s
my-nginx-54cddf7468-jl5lt   1/1     Running   0          9m11s

$ k scale deploy/my-nginx --replicas=3
deployment.apps/my-nginx scaled
$ k get po
NAME                        READY   STATUS    RESTARTS   AGE
my-nginx-54cddf7468-9qv6b   1/1     Running   0          10m
my-nginx-54cddf7468-f8l6f   0/1     Running   0          4s # Some times it takes a bit of time
my-nginx-54cddf7468-jl5lt   1/1     Running   0          10m
$ k get po
NAME                        READY   STATUS    RESTARTS   AGE
my-nginx-54cddf7468-9qv6b   1/1     Running   0          11m
my-nginx-54cddf7468-f8l6f   1/1     Running   0          32s # <- Now the Pod is ready
my-nginx-54cddf7468-jl5lt   1/1     Running   0          11m

$ k apply -f fundamentals/Deployment/nginx_deployment_spec.yaml # Now we are going back to the cluster state described in the YAML
deployment.apps/my-nginx configured
$ k get po
NAME                        READY   STATUS    RESTARTS   AGE
my-nginx-54cddf7468-9qv6b   1/1     Running   0          12m
my-nginx-54cddf7468-jl5lt   1/1     Running   0          12m
```
